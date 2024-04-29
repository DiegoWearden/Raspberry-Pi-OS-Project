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
        uint32_t *saved_SP;
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

        uint32_t stack[2048]; // 8 kb = (8 * 1024 / sizeof(uint32_t))
        TCBWithWork(Work work) : work(work)
        {
            auto bottomIndex = 2048 - 1;
            stack[bottomIndex] = (uint32_t)entry; // return address
            stack[bottomIndex - 1] = 0;
            stack[bottomIndex - 2] = 0;
            stack[bottomIndex - 3] = 0;
            stack[bottomIndex - 4] = 0;
            saved_SP = &stack[bottomIndex - 4];
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
    // using namespace alogx;
    alogx::clearZombies();
    auto tcb = new alogx::TCBWithWork(work);
    alogx::readyQ.add(tcb);
}

#endif
