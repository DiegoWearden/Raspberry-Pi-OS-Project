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
        uint64_t* stackPointer;  // Changed from uint32_t* to uint64_t*
        TCBBase* next;
        TCBBase* prev;

        virtual void doWork() = 0;

        virtual ~TCBBase() { }
    };

    extern void entry();

    template <typename Work>
    struct TCB : public TCBBase {
        Work work;
        uint64_t the_stack[(8 * 1024) / sizeof(uint64_t)];  // Change the stack array to uint64_t

        TCB(Work work) : work(work) {
            printf("Inside of TCB constructor\n");
            
            auto bi = ((8 * 1024) / sizeof(uint64_t)) - 1;  // Adjust the indexing for 64-bit width

            // Adjusting initialization to ensure 64-bit compatibility and proper alignment
            the_stack[bi] = 0;  // Cast the function pointer to uint64_t
            the_stack[bi-1] = 0;
            the_stack[bi-2] = 0;
            the_stack[bi-3] = 0;
            the_stack[bi-4] = (uint64_t) entry;
            stackPointer = &the_stack[bi-4];  // Adjust the stack pointer initialization
            printf("stackPointer value: %x\n", stackPointer);
            printf("entry: %x\n", (uint64_t) entry);
        }

        void doWork() override {
            printf("right before work\n");
            work();
            printf("right after work\n");
        }
    };

    extern Queue<TCBBase> ready_queues;
    extern void deleteQueue();
};

template <typename T>
void thread(T work) {
    using namespace dwearden;
    // printf("creating TCB on heap...\n");
    auto tcb = new TCB(work);  // Ensure template usage is clear
    ready_queues.add(tcb);
    // printf("added thread to ready queue\n");
}

#endif
