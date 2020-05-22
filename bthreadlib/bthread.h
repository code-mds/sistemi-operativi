//
// Created by massi on 19.03.2020.
//

#ifndef TEST_C_BTHREAD_H
#define TEST_C_BTHREAD_H

typedef unsigned long int bthread_t;

typedef enum {
    __BTHREAD_ROUND_ROBIN = 0,
    __BTHREAD_PRIORITY = 1,
    __BTHREAD_RANDOM = 2,
    __BTHREAD_LOTTERY = 3
} bthread_scheduling_policy;

typedef struct {
} bthread_attr_t;

typedef void *(*bthread_routine) (void *);
typedef void *(*bthread_scheduling_routine) (void *);

int bthread_create(bthread_t *bthread,
                    const bthread_attr_t *attr,
                    void *(*start_routine) (void *),
                    void *arg);
int bthread_join(bthread_t bthread, void **retval, bthread_scheduling_policy policy);
void bthread_yield();
void bthread_exit(void *retval);
void bthread_sleep(double ms);
void bthread_cancel(bthread_t bthread);
void bthread_testcancel();
void bthread_printf(const char* format, ...);
#endif //TEST_C_BTHREAD_H
