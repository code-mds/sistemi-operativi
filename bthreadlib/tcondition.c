//
// Created by massi on 23.05.2020.
// https://linux.die.net/man/3/pthread_cond_wait
//

#include <assert.h>
#include <stddef.h>
#include "tcondition.h"
#include "bthread_private.h"

/**
 * Initialize a Condition object
 * @param c condition object
 * @param attr (ignored)
 */
int bthread_cond_init(bthread_cond_t *c, const bthread_condattr_t *attr) {
    assert(c != NULL);
    c->waiting_list = NULL;
    return 0;
}

int bthread_cond_destroy(bthread_cond_t *c) {
    assert(tqueue_size(c->waiting_list) == 0);
    return 0;
}

/**
 * block on a condition variable.
 * It shall be called with mutex locked by the calling thread
 * @param c
 * @param mutex
 * @return
 */
int bthread_cond_wait(bthread_cond_t *c, bthread_mutex_t *mutex) {
    // To avoid race-conditions we must execute mutex procedures atomically with the timer signal disabled.
    bthread_block_timer_signal();

    __bthread_scheduler_private* scheduler = bthread_get_scheduler();
    volatile __bthread_private* bthread = (__bthread_private*)tqueue_get_data(scheduler->current_item);
    bthread->state = __BTHREAD_BLOCKED;
    tqueue_enqueue(&c->waiting_list, (void*)bthread);

    // release the lock before going to sleep, so lock is available for signaling thread
    bthread_mutex_unlock(mutex);
    bthread_yield();
    return 0;
}

int bthread_cond_signal(bthread_cond_t *c) {
    bthread_block_timer_signal();

    __bthread_private* unlock = tqueue_pop(&c->waiting_list);
    if (unlock != NULL) {
        unlock->state = __BTHREAD_READY;
        bthread_yield();
        return 0;
    }

    bthread_unblock_timer_signal();
    return 0;
}

int bthread_cond_broadcast(bthread_cond_t *c) {
    bthread_block_timer_signal();

    __bthread_private* unlock = tqueue_pop(&c->waiting_list);
    while (unlock != NULL) {
        unlock->state = __BTHREAD_READY;
        unlock = tqueue_pop(&c->waiting_list);
    }

    bthread_yield();
    return 0;
}
