// Light-weight locking tools.
// Author: chenlinc@cse.ohio-state.edu

#ifndef ATOMIC_H_
#define ATOMIC_H_

void get_lock(volatile int *lock_value) {   
    while(!(__sync_val_compare_and_swap(lock_value, 0, 1) == 0))
      ;
}   

void release_lock(volatile int *lock_value) {   
    __sync_val_compare_and_swap(lock_value, 1, 0); 
}   

#endif  // ATOMIC_H_
