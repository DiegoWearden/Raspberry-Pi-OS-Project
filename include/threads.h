#ifndef _threads_h_
#define _threads_h_

#include "queue.h"
#include "heap.h"

extern void threadsInit();

extern void stop();
extern void yield();

namespace dwearden {

    struct TCBBase {

        bool finished = false;

        uint32_t* stackPointer;

        TCBBase* next;

        TCBBase* prev;

        virtual void doWork() = 0;

        virtual ~TCBBase(){
            
        }

    };

    extern void entry();

    template <typename Work>
    struct TCB : public TCBBase {
        Work work;

        uint32_t the_stack[(8 * 1024) / sizeof(uint32_t)];

        TCB(Work work) : work(work) {
            printf("stuck here4????\n");
            
            auto bi = ((8 * 1024) / sizeof(uint32_t)) - 1;

            the_stack[bi] = (uint32_t) entry;
            the_stack[bi-1] = 0;
            the_stack[bi-2] = 0;
            the_stack[bi-3] = 0;
            the_stack[bi-4] = 0;
            stackPointer = &the_stack[bi-4];
        }

        void doWork() override {
            work();
        }
    };

    extern Queue<TCBBase> ready_queues;
    extern void deleteQueue();


};

template <typename T>
void thread(T work) {
    using namespace dwearden;
    TCB tcb(work);
    ready_queues.add(&tcb);
}

#endif