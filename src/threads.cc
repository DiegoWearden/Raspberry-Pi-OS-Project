#include "printf.h"
#include "smp.h"
#include "stdint.h"
#include "threads.h"
#include "queue.h"
#include "utils.h"
// #include "heap.h"

// ------------
// threads.cc -
// ------------

extern "C" void context_switch(uint64_t **saveOldSpHere, uint64_t *newSP);
// global ready queue
namespace alogx
{
    Queue<TCB> readyQ{};                // Queue of threads ready to run
    Queue<TCB> zombieQ{};               // Queue of threads ready to be deleted
    TCB *runningThreads[4] = {nullptr}; // use an array for multiple cores, index with SMP::me()
    TCB *oldThreads[4] = {nullptr};     // save the oldThread before context switching
    Queue<TCB> *addMeHere[4];           // save the queue I want to be added to before context switching
                                        //  /*ISL*/ InterruptSafeLock *myLock[4];        // save the isl of the thread
    bool oldState[4];                   // save interrupt state

    struct DummyThread : public TCB
    {
        void doTheWork() override
        {
            // PANIC("should never go here");
        }
    };

    void entry()
    {
        // ASSERT(Interrupts::isDisabled());
        auto me = get_core_number();
        //  ASSERT(runningThreads[me] != nullptr);
        auto thread = runningThreads[me];
        thread->done.set(false);
        thread->wasDisabled = false;
        restoreState();
        thread->doTheWork();
        stop();
    }
};

void threadsInit() // place to put any intialization logic
{
    using namespace alogx;
    for (int i = 0; i < 4; i++)
    {
        printf("dummy #%d\n", i);
        runningThreads[i] = new DummyThread();
        printf("dummy created \n");
        runningThreads[i]->done.set(true);
    }
}

void yield()
{
    alogx::block(&alogx::readyQ);
}

void stop()
{
    using namespace alogx;
    clearZombies();
    while (true)
    {
        block(&zombieQ);
    }
    // PANIC("Stop returning OH NO \n");
}

namespace alogx
{
    void block(Queue<TCB> *q)
    {
        using namespace alogx;
        // bool was = Interrupts::disable();
        auto nextThread = alogx::readyQ.remove();
        if (nextThread == nullptr) // no other threads. I can keep working/spinning
        {
            // if (isl)
            // {
            //     isl->unlock();
            // }
            // Interrupts::restore(was);
            return;
        }
        int me = get_core_number();
        //  ASSERT(alogx::runningThreads[me] != nullptr);

        oldThreads[me] = runningThreads[me]; // save the old thread
                                             // oldThreads[me]->wasDisabled = was;   // save the interrupt state prior to block
                                             // oldState[me] = was;                  // save the interrupt state prior to block
        addMeHere[me] = q;                   // save the queue to add me on
        // myLock[me] = isl;                    // save my lock

        alogx::runningThreads[me] = nextThread; // This is a lie, only observable by the core. its ok ?
        printf("switching thread\n");
        context_switch(&oldThreads[me]->saved_SP, nextThread->saved_SP);
        //  ASSERT(Interrupts::isDisabled());
        restoreState();
    }

    void restoreState()
    {
        // ASSERT(Interrupts::isDisabled());
        int me = get_core_number();
        auto prev = oldThreads[me];
        // auto running = runningThreads[me];
        if (prev)
        {
            if (addMeHere[me] != nullptr)
            {
                addMeHere[me]->add(prev);
            }
            // I have successfully added my prev to desired queue,
            // I can unlock the lock if I was given one
            // if (myLock[me] != nullptr)
            // {
            // myLock[me]->unlock();
            // Interrupts::restore(oldThreads[me]->wasDisabled);
            // }
            // Interrupts::restore(prev->wasDisabled);
        }
        // if (running->wasDisabled)
        // {
        //     cli(); // disable
        // }
        // else
        // {
        //     Interrupts::restore(running->wasDisabled);
        // }
    }

    void clearZombies()
    {
        printf("clearing\n");
        auto killMe = zombieQ.remove();
        while (killMe != 0)
        {
            // bool was = Interrupts::disable();
            delete killMe;
            // Interrupts::restore(was);
            killMe = zombieQ.remove();
        }
        printf("done clearing\n");
    }
};