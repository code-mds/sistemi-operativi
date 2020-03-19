//
// Created by massi on 19.03.2020.
//

#ifndef TEST_C_BTHREAD_PRIVATE_H
#define TEST_C_BTHREAD_PRIVATE_H

#include "bthread.h"

typedef enum {
    __BTHREAD_READY = 0,
    __BTHREAD_BLOCKED,
    __BTHREAD_SLEEPING,
    __BTHREAD_ZOMBIE
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
} __bthread_private;

#endif //TEST_C_BTHREAD_PRIVATE_H