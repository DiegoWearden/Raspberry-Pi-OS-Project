#ifndef _queue_h_
#define _queue_h_

#include "atomic.h"

template <typename T>
class Queue
{
    T *volatile first = nullptr;
    T *volatile last = nullptr;
    // LockType lock;

public:
    Queue() : first(nullptr), last(nullptr)
    {
    }
    Queue(const Queue &) = delete;

    // void monitor_add()
    // {
    //     monitor((uintptr_t)&last);
    // }

    // void monitor_remove()
    // {
    //     monitor((uintptr_t)&first);
    // }

    void add(T *t)
    {
        printf("adding to queue\n");
        // LockGuard g{lock};
        printf("t adr %x\n", t);
        t->next = nullptr;
        printf("first %x\n", first);
        if (first == nullptr)
        {
            printf("first is null\n");
            first = t;
        }
        else
        {
            printf("first is not null\n");
            last->next = t;
        }
        last = t;
        printf("done adding to queue\n");
    }

    T *remove()
    {
        // LockGuard g{lock};
        if (first == nullptr)
        {
            return nullptr;
        }
        auto it = first;
        first = it->next;
        if (first == nullptr)
        {
            last = nullptr;
        }
        return it;
    }

    T *remove_all()
    {
        // LockGuard g{lock};
        auto it = first;
        first = nullptr;
        last = nullptr;
        return it;
    }
};

#endif
