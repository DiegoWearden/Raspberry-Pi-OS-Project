
// volatile int lockingthing = 0;

// void acquire_lock() {
//     while (__sync_lock_test_and_set(&lockingthing, 1)) {
//     }
// }

// void release_lock() {
//     __sync_lock_release(&lockingthing);  // Set the lock back to 0
// }