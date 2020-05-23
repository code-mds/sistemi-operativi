//
// Created by massi on 19.03.2020.
//

#ifndef BTHREAD_PRIVATE_H
#define BTHREAD_PRIVATE_H

#include <setjmp.h>
#include "bthread.h"
#include "tqueue.h"

typedef enum {
    __BTHREAD_READY = 0,
    __BTHREAD_BLOCKED,
    __BTHREAD_SLEEPING,
    __BTHREAD_ZOMBIE,
    __BTHREAD_EXIT
} bthread_state;

typedef struct {
    bthread_t tid;
    bthread_routine body;
    void* arg;
    bthread_state state;
    bthread_attr_t attr;
    char* stack;
    jmp_buf context;
    void* retval;
    double wake_up_time;
    int cancel_req;
} __bthread_private;

typedef struct {
    TQueue queue;
    TQueue current_item;
    jmp_buf context;
    bthread_t current_tid;
    bthread_scheduling_routine scheduling_routine;
} __bthread_scheduler_private;

// Private methods
__bthread_scheduler_private* bthread_get_scheduler();
static int bthread_check_if_zombie(bthread_t bthread, void **retval);
static TQueue bthread_get_queue_at(bthread_t bthread);
void bthread_cleanup();

void bthread_block_timer_signal();
void bthread_unblock_timer_signal();

void set_timer(int priority);
void policy_priority();
void policy_random();
void policy_round_robin();
void policy_lottery();

#endif //BTHREAD_PRIVATE_H
