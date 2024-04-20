#ifndef _CRITICAL_H_
#define _CRITICAL_H_

#include "atomic.h"
#include "printf.h"
#include "utils.h"

// Implements a critical section, only one core can be in the critical
// section at a time.
//

extern Atomic<int> critical_flag;
extern volatile uint32_t critical_depth;
extern volatile uint32_t critical_owner;
// static Barrier barrier;

template <typename Work>
inline void critical(Work work) {
    uint32_t thread_id = get_core_number();
    if (critical_owner != thread_id) {
        critical_flag.exchange(1);
        printf("gets here\n");
        critical_owner = thread_id;
    }
    critical_depth++;
    work();
    critical_depth--;
    if (critical_depth == 0) {
        critical_owner = uint32_t(-1);
        critical_flag.exchange(0);
    }
}

// how to identify core SMP::me()

/*
wait_until free
work()
im_free
*/
#endif
