//
// Created by massi on 23.05.2020.
// example: https://www.geeksforgeeks.org/use-posix-semaphores-c/
//

#include <assert.h>
#include <stddef.h>
#include "tsemaphore.h"

/**
 * Initialize a semaphore
 * @param m the semaphore to initialize
 * @param pshared (ignored in bthread)
 * @param value value to assign to the semaphore m
 * @return
 */
int bthread_sem_init(bthread_sem_t *m, int pshared, int value) {
    assert(m != NULL);
    m->value = value;
    m->waiting_list = NULL;

    return 0;
}

int bthread_sem_destroy(bthread_sem_t *m) {
    assert(tqueue_size(m->waiting_list) == 0);
    return 0;
}

/*
 * Lock a semaphore or wait
 */
int bthread_sem_wait(bthread_sem_t *m) {
    //TODO
    return 0;
}

/*
 * Release or signal a semaphore
 */
int bthread_sem_post(bthread_sem_t *m) {
    //TODO
    return 0;
}
