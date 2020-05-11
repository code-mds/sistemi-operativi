//
// Created by massi on 28.03.2020.
//
#include <stdio.h>
#include "../bthread.h"
#define THREADS 3

void test_bthread_base(void *(*start_routine)(void *), int *loops) {
    bthread_t tid[THREADS];
    for (int i = 0; i < THREADS; ++i) {
        bthread_create(&tid[i], NULL, start_routine, (void*)loops[i]);
        fprintf(stdout, "%i) thread_%lu created, # of loops: %d \n", i, tid[i], loops[i]);
    }

    int failed = 0;
    for (int i = 0; i < THREADS; ++i) {
        int retval = -1;
        fprintf(stdout, "%d) thread_%lu join, # of loops: %d \n", i, tid[i], loops[i]);
        bthread_join(tid[i], (void**)&retval);
        if(loops[i] != retval && retval != -1) {
            fprintf(stderr, "test FAILED %d != %d \n", loops[i], retval);
            failed = 1;
        }
    }

    if(!failed)
        fprintf(stdout, "test PASSED\n");
}

/* simple test */
void* my_routine(void* param) {
    int loops = (int)param;
    int i=0, c;
    for (; i < loops; ++i) {
        c=i;
    }

    return (void*)i;
}

void test_bthread_simple() {
    fprintf(stdout, "** test_bthread_simple **\n");

    int loops[THREADS] = {1000000, 2000000, 3000000};
    test_bthread_base(my_routine, loops);
}

/* sleep test */
void* my_routine_sleep(void* param) {
    int loops = (int)param;
    int i=0, c;
    for (; i < loops; ++i) {
        bthread_sleep(100);
        c=i;
    }

    return (void*)i;
}

void test_bthread_sleep() {
    fprintf(stdout, "** test_bthread_sleep **\n");

    int loops[THREADS] = { 5, 10, 8 };
    test_bthread_base(my_routine_sleep, loops);
}

/* cancel test */
void* my_routine_cancel(void* param) {
    int loops = (int)param;
    int i=0;
    for (; i < loops; ++i) {
        fprintf(stdout, "thread [%d]  -> %d\n", loops, i);
        bthread_testcancel();
    }

    // stop first thread
    bthread_cancel(0);

    return (void*)i;
}

void test_bthread_cancel() {
    fprintf(stdout, "** test_bthread_cancel **\n");

    int loops[THREADS] = { 20, 10, 8 };
    test_bthread_base(my_routine_cancel, loops);
}

/* yield test */
void* my_routine_yield(void* param) {
    int loops = (int)param;
    int i=0;
    for (; i < loops; ++i) {
        fprintf(stdout, "thread [%d]  -> %d\n", loops, i);
        bthread_yield();
    }

    return (void*)i;
}
void test_bthread_yield() {
    fprintf(stdout, "** test_bthread_yield **\n");

    int loops[THREADS] = { 20, 10, 8 };
    test_bthread_base(my_routine_yield, loops);
}