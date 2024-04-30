#include "queue.h"
#include "threads.h"
#include "heap.h"
#include "printf.h"
#include "context_switch.h"

namespace dwearden {
    Queue<TCBBase> ready_queues;
    TCBBase** running_threads = new TCBBase*[1]{};
    Queue<TCBBase> deadQueue;

    struct DummyThread: public TCBBase {

        void doWork() override {
            panic("should never get here");
        }

    };

    void deleteQueue(){
        auto thread = deadQueue.remove();
        while(thread != nullptr){
            delete thread;
            thread = deadQueue.remove();
        }
    }
    

    void entry() {
    printf("right after context switch\n");
    uint32_t core = get_core_number();

    if(running_threads[core]->prev && !running_threads[core]->prev->finished) {
        ready_queues.add(running_threads[core]->prev);
    } else {
        deadQueue.add(running_threads[core]->prev);
    }
        auto me = running_threads[get_core_number()];
        ASSERT(me != nullptr);
        me->doWork();
        stop();
    }
};

void threadsInit() {
    using namespace dwearden;
    for (uint32_t i = 0; i < 1; i++) {
        // printf("allocating dummy threads...\n");
        running_threads[i] = new DummyThread();
    }
}

void yield() {
    using namespace dwearden;
    auto me = get_core_number();
    auto next_thread = ready_queues.remove();
    if (next_thread == nullptr) {
        return;
    }
    ASSERT(running_threads[me] != nullptr);
    auto old_thread = running_threads[me];
    next_thread->prev = old_thread;
    running_threads[me] = next_thread;

    printf("right before context switch. still need to implement...\n");
    printf("&old_thread->stackPointer: 0x%x next_thread->stackPointer: 0x%x", &old_thread->stackPointer, next_thread->stackPointer);
    context_switch(&old_thread->stackPointer, next_thread->stackPointer);
    // printf("right after context switch\n");

    me = get_core_number();

    if(running_threads[me]->prev && !running_threads[me]->prev->finished) {
        ready_queues.add(running_threads[me]->prev);
    } else {
        deadQueue.add(running_threads[me]->prev);
    }
}


void stop() {
    using namespace dwearden;
    deleteQueue();
    running_threads[get_core_number()]->finished = true;
    while (true) {
        yield();
    }
}