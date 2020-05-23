//
// Created by massi on 23.05.2020.
//

#include <assert.h>
#include <stddef.h>
#include "tcondition.h"

int bthread_cond_init(bthread_cond_t *c, const bthread_condattr_t *attr) {
    assert(c != NULL);
    c->waiting_list = NULL;
    return 0;
}

int bthread_cond_destroy(bthread_cond_t *c) {
    assert(tqueue_size(c->waiting_list) == 0);
    return 0;
}

int bthread_cond_wait(bthread_cond_t *c, bthread_mutex_t *mutex) {
    //TODO
    return 0;
}

int bthread_cond_signal(bthread_cond_t *c) {
    //TODO
    return 0;
}

int bthread_cond_broadcast(bthread_cond_t *c) {
    //TODO
    return 0;
}
