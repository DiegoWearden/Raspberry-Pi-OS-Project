#ifndef _threads_h_
#define _threads_h_

#include "atomic.h"
#include "queue.h"
#include "heap.h"
#include "printf.h"
#include "smp.h"
#include "utils.h"

extern void threadsInit();

extern void stop();
extern void yield();
namespace alogx
{

    struct TCB
    {
        TCB *next;
        uint64_t *saved_SP;
        bool wasDisabled = false;
        Atomic<int> done{false};

        virtual void doTheWork() = 0; // Abstract/virtual function that must be overridden
        virtual ~TCB(){};             // Allows child classes to be deleted
    };

    extern void block(Queue<TCB> *q /*ISL*/); // generalized yield
    extern void entry();                      // have a thread start work
    extern void restoreState();               // restore state post context switch
    extern void clearZombies();               // delete threads that have called stop.

    template <typename Work>
    struct TCBWithWork : public TCB
    {
        Work work;

        uint64_t stack[2048]; // 8 kb = (8 * 1024 / sizeof(uint32_t))
        TCBWithWork(Work work) : work(work)
        {
            auto bottomIndex = 2048 - 1;
            stack[bottomIndex] = 0;
            stack[bottomIndex - 1] = 0;
            stack[bottomIndex - 2] = 0;
            stack[bottomIndex - 3] = 0;
            stack[bottomIndex - 4] = 0;
            stack[bottomIndex - 5] = 0;
            stack[bottomIndex - 6] = 0;
            stack[bottomIndex - 7] = 0;
            stack[bottomIndex - 8] = 0;
            stack[bottomIndex - 9] = 0;
            stack[bottomIndex - 10] = 0;
            stack[bottomIndex - 11] = (uint64_t)entry; // return address in x30

            saved_SP = &stack[bottomIndex - 11];
            // state is what the thread is doing
            done.set(false);
        }

        void doTheWork() override
        {
            // sti();
            work();
        }
    };

    extern Queue<TCB> readyQ;
    extern Queue<TCB> zombieQ;
};

template <typename T>
void thread(T work)
{
    printf("creating a thread\n");
    // using namespace alogx;
    alogx::clearZombies();
    printf("creating TCB with work\n");
    // auto tcb alogx::TCBWithWork(work);
    auto tcb = new alogx::TCBWithWork(work);
    printf(" TCB created\n");
    alogx::readyQ.add(tcb);
    printf("added to the queue");
}

#endif
