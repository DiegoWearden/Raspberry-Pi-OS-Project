#ifndef _CRITICAL_H_
#define _CRITICAL_H_

#include "atomic.h"
#include "printf.h"
#include "utils.h"

// Implements a critical section, only one core can be in the critical
// section at a time.
//

extern Atomic<bool> critical_flag;
extern volatile uint32_t critical_depth;
extern volatile uint32_t critical_owner;
// static Barrier barrier;

template <typename Work>
inline void critical(Work work)
{
    // Do the work in a critical section
    // MISSING();

    /*
    Plan:
    Check if we are nested,
    check if this is the first time entering
    else have to wait for other core to finish its critical work

    Then we check if we are the outermost nesting, if we are then we unlock after calling work

    // volatile always writes back to memory. stops compiler from opimizing out or putting val in reg and not writing back
    */

    //  /*
    // check if I can enter critical section, check if the entry is the first time, and if no one owns the section
    if (!critical_flag.exchange(true) && critical_depth == 0 && critical_owner == uint32_t(-1)) // first time entry
    {
        critical_flag.exchange(true);       // set flag to true to mark locked
        critical_owner = get_core_number(); // set who owns the critical section
        critical_depth++;                   // depth of nesting increased
        work();
        critical_depth--; // depth decreased after work finished
    }
    // check if I can enter critical section, check if the entry is nested, and if I am the owner
    else if (critical_depth > 0 && critical_owner == get_core_number()) // nested entry
    {
        critical_depth++;
        work();
        critical_depth--; // work done, one nest layer less
    }
    else // waiting for for critical section to be unlocked
    {
        while (critical_flag.exchange(true)) // wait for other core to finish
            ;
        critical_flag.set(true); // lock critical section
        critical_owner = get_core_number();
        critical_depth++;
        work();
        critical_depth--;
    }

    if (critical_depth == 0 && critical_owner == get_core_number()) // unlock
    {
        // exiting critical section, reset values
        critical_flag.exchange(false);
        critical_depth = uint32_t(0);
        critical_owner = uint32_t(-1);
    }
    // */

    // spinLock.lock();
    // work();
    // spinLock.unlock();
}

// how to identify core SMP::me()

/*
wait_until free
work()
im_free
*/
#endif
