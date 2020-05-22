//
// Created by massi on 28.03.2020.
//
#include <stdio.h>
#include <stdlib.h>
#include "test_bthread.h"

#define THREADS 3
bthread_scheduling_policy _schedulingPolicy = __BTHREAD_ROUND_ROBIN;

/*
 * ROUND ROBIN scheduling policy
 */
void test_round_robin_scheduling() {
    fprintf(stdout, "\n*******************************\n");
    fprintf(stdout, "START bthread ROUND ROBIN scheduling\n");
    fprintf(stdout, "*******************************\n");
    start_test(__BTHREAD_ROUND_ROBIN);
}

/*
 * RANDOM scheduling policy
 */
void test_random_scheduling() {
    fprintf(stdout, "\n*******************************\n");
    fprintf(stdout, "START bthread RANDOM scheduling\n");
    fprintf(stdout, "*******************************\n");
    start_test(__BTHREAD_RANDOM);
}

/*
 * PRIORITY scheduling policy
 */
void test_priority_scheduling() {
    fprintf(stdout, "\n*******************************\n");
    fprintf(stdout, "START bthread PRIORITY scheduling\n");
    fprintf(stdout, "*******************************\n");
    start_test(__BTHREAD_PRIORITY);
}

/*
 * LOTTERY scheduling policy
 */
void test_lottery_scheduling() {
    fprintf(stdout, "\n*******************************\n");
    fprintf(stdout, "START bthread LOTTERY scheduling\n");
    fprintf(stdout, "*******************************\n");
    start_test(__BTHREAD_LOTTERY);
}

/*
 * set the policy and invoke the base tests
 */
void start_test(bthread_scheduling_policy schedulingPolicy) {
    _schedulingPolicy = schedulingPolicy;
    test_bthread_simple();
    test_bthread_cancel();
    test_bthread_yield();
    test_bthread_sleep();
}

/*
 * Base method for specialized tests
 */
void test_bthread_base(void *(*start_routine)(void *), int *loops) {
    bthread_t tid[THREADS];
    for (int i = 0; i < THREADS; ++i) {
        bthread_attr_t attr;
        attr.priority = __BTHREAD_PRIORITY_LOW + i;
        bthread_create(&tid[i], &attr, start_routine, (void*)loops[i]);
        fprintf(stdout, "%i) thread_%lu created, # of loops: %d \n", i, tid[i], loops[i]);
    }

    for (int i = 0; i < THREADS; ++i) {
        int retval = -1;
        fprintf(stdout, "%d) thread_%lu join, # of loops: %d \n", i, tid[i], loops[i]);
        bthread_join(tid[i], (void**)&retval, _schedulingPolicy);
        if(loops[i] != retval && retval != -1) {
            fprintf(stderr, "test FAILED %d != %d \n", loops[i], retval);
            exit(1);
        }
    }

    fprintf(stdout, "test PASSED\n");
}

/*
 * simple routine
 */
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

/*
 * routine for sleep test
 */
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

/*
 * routine for cancel test
 */
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

/*
 * routine for yield test
 */
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