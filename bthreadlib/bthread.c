//
// Created by massi on 22.03.2020.
//
#include <stddef.h>
#include <stdlib.h>
#include "bthread.h"
#include "bthread_private.h"

__bthread_scheduler_private * _scheduler = NULL;

void bthread_cleanup() {
    free(_scheduler);
    _scheduler = NULL;
}

__bthread_scheduler_private *bthread_get_scheduler() {
    if(_scheduler == NULL)
        _scheduler = malloc(sizeof(__bthread_scheduler_private));

    return _scheduler;
}

int bthread_create(bthread_t *bthread, const bthread_attr_t *attr, void *(*start_routine)(void *), void *arg) {
    __bthread_private* thread = malloc(sizeof(__bthread_private));
    thread->tid = tqueue_enqueue(&bthread_get_scheduler()->queue, thread);
    *bthread = thread->tid;
    thread->body = start_routine;
    thread->state = __BTHREAD_READY;
    thread->stack = NULL;

    //TODO
    //thread->attr = *attr;
    //thread->arg =
    //thread->retval =

    return 0;
}

int bthread_join(bthread_t bthread, void **retval) {
    //TODO
    return 0;
}

void bthread_yield() {
    //TODO
}

void bthread_exit(void *retval) {
    //TODO
}
