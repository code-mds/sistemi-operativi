//
// Created by massi on 23.05.2020.
//

#ifndef TCONDITION_H
#define TCONDITION_H

#include "tqueue.h"
#include "tmutex.h"

typedef struct {
    TQueue waiting_list;
} bthread_cond_t;

// Defined only for "compatibility" with pthread
typedef struct {} bthread_condattr_t;

// attr is ignored
int bthread_cond_init(bthread_cond_t* c, const bthread_condattr_t *attr);

int bthread_cond_destroy(bthread_cond_t* c);
int bthread_cond_wait(bthread_cond_t* c, bthread_mutex_t* mutex);
int bthread_cond_signal(bthread_cond_t* c);
int bthread_cond_broadcast(bthread_cond_t* c);

#define bthread_cond_notify(s) \
        bthread_cond_signal(s);

#define bthread_cond_notifyall(s) \
        bthread_cond_broadcast(s);

#endif //TCONDITION_H
