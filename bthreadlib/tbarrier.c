//
// Created by massi on 23.05.2020.
// https://linux.die.net/man/3/pthread_barrier_wait
//

#include <assert.h>
#include <stddef.h>
#include "tbarrier.h"
#include "bthread_private.h"

/**
 * Initialize a Barrier object
 * @param b
 * @param attr (ignored)
 * @param count
 */
int bthread_barrier_init(bthread_barrier_t *b, const bthread_barrierattr_t *attr, unsigned int count) {
    assert(b != NULL);
    b->barrier_size = 0;
    b->count = count;
    b->waiting_list = NULL;

    return 0;
}

int bthread_barrier_destroy(bthread_barrier_t *b) {
    //TODO
    assert(b != NULL);
    assert(b->barrier_size == 0);
    assert(tqueue_size(b->waiting_list) == 0);
    return 0;
}

/**
 * The bthread_barrier_wait() function shall synchronize participating threads at the barrier referenced by barrier.
 * The calling thread shall block until the required number of threads have called bthread_barrier_wait() specifying the barrier.
 * @param b the barrier object
 */
int bthread_barrier_wait(bthread_barrier_t *b) {
    // To avoid race-conditions we must execute mutex procedures atomically with the timer signal disabled.
    bthread_block_timer_signal();

    __bthread_scheduler_private* scheduler = bthread_get_scheduler();
    volatile __bthread_private* bthread = (__bthread_private*)tqueue_get_data(scheduler->current_item);
    if (b->count == b->barrier_size) {
        // When the barrier size is reached we loop throught the waiting list and set each thread to READY
        __bthread_private* unlock = tqueue_pop(&b->waiting_list);
        while (unlock != NULL) {
            unlock->state = __BTHREAD_READY;
            unlock = tqueue_pop(&b->waiting_list);
        }
        b->barrier_size = 0;
        bthread_unblock_timer_signal();
    } else {
        b->barrier_size++;
        bthread->state = __BTHREAD_BLOCKED;
        // We re-use our queue to store the list of threads that need to be synchronized
        tqueue_enqueue(&b->waiting_list, (void*)bthread);
        bthread_yield();
    }

    return 0;
}
