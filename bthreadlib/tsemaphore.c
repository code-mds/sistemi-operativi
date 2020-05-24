//
// Created by massi on 23.05.2020.
// https://linux.die.net/man/7/sem_overview
// example: https://www.geeksforgeeks.org/use-posix-semaphores-c/
//

#include <assert.h>
#include <stddef.h>
#include "tsemaphore.h"
#include "bthread_private.h"

/**
 * Initialize a Semaphore object
 * A semaphore is an integer whose value is never allowed to fall below zero
 * @param s the semaphore to initialize
 * @param pshared (ignored in bthread)
 * @param value value to assign to the semaphore s
 */
int bthread_sem_init(bthread_sem_t *s, int pshared, int value) {
    assert(s != NULL);
    assert(value >= 0);
    s->value = value;
    s->waiting_list = NULL;

    return 0;
}

int bthread_sem_destroy(bthread_sem_t *s) {
    assert(s != NULL);
    assert(tqueue_size(s->waiting_list) == 0);
    return 0;
}

/*
 * Decrement the semaphore value by one (Lock or wait)
 * If the value of a semaphore is currently zero,
 * then a sem_wait operation will block until the value becomes greater than zero.
 */
int bthread_sem_wait(bthread_sem_t *s) {
    bthread_block_timer_signal();

    if(s->value == 0) {
        __bthread_scheduler_private* scheduler = bthread_get_scheduler();
        volatile __bthread_private* bthread = (__bthread_private*)tqueue_get_data(scheduler->current_item);
        bthread->state = __BTHREAD_BLOCKED;
        tqueue_enqueue(&s->waiting_list, (void*)bthread);
        bthread_yield();
    } else {
        s->value--;
    }

    bthread_unblock_timer_signal();
    return 0;
}

/*
 * Increment the semaphore value by one (Release or signal a semaphore)
 */
int bthread_sem_post(bthread_sem_t *s) {
    bthread_block_timer_signal();

    __bthread_private* unlock = tqueue_pop(&s->waiting_list);
    if (unlock != NULL) {
        unlock->state = __BTHREAD_READY;
        bthread_yield();
        return 0;
    } else {
        s->value++;
    }

    bthread_unblock_timer_signal();
    return 0;
}
