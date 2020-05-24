//
// Created by massi on 23.05.2020.
//
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "test_criticalsection.h"

#define NUM_LOOPS 10000000
static const int NumThreads = 2;

// shared data
static long long sum;
static bthread_mutex_t my_mutex;

void* thread_proc_nomutex(void* arg) {
    int param = *(int*)arg;
    for (int i = 0; i < NUM_LOOPS; ) {
        for (int j = 0; j < 500; ++j) {
            i++;
            sum += param;
        }
    }
    return NULL;
}

void* thread_proc_mutex(void* arg) {
    int param = *(int*)arg;
    for (int i = 0; i < NUM_LOOPS; ) {
        // critical section, lock/unlock every 50 increment
        bthread_mutex_lock(&my_mutex);
        for (int j = 0; j < 500; ++j) {
            i++;
            sum += param;
        }
        bthread_mutex_unlock(&my_mutex);
        // end critical section
    }
    return NULL;
}

void test_mutex() {
    for (int i = 0; i < 10; ++i) {
        test_sum(thread_proc_mutex);
    }
}

void test_nomutex() {
    for (int i = 0; i < 10; ++i) {
        test_sum(thread_proc_nomutex);
    }
}

/*
 * Start 2 threads. First increment by 1 a shared value (sum) and the second decrement by 1
 * Each thread run a for loop of NUM_LOOPS. At the end we expect a 0 balance
 */
void test_sum(bthread_scheduling_routine thread_proc) {
    sum = 0;
    bthread_mutex_init(&my_mutex, NULL);

    int params[NumThreads];
    bthread_t tid[NumThreads];
    for (int i = 0; i < NumThreads; ++i) {
        params[i] = i%2 ? -1 : 1;

        bthread_attr_t attr;
        attr.priority = __BTHREAD_PRIORITY_LOW;
        bthread_create(&tid[i], &attr, thread_proc, &params[i]);
        fprintf(stdout,"%i) thread_%lu created, param: %d \n", i, tid[i], params[i]);
    }

    for (volatile int i = 0; i < NumThreads; ++i) {
        int retval = -1;
        bthread_join(tid[i], (void**)&retval);
    }

    long long expecetd_result = 0;
    if(sum != expecetd_result) {
        fprintf(stderr, "test FAILED expected(%lld) found(%lld) \n", expecetd_result, sum);
        exit(1);
    }

    bthread_cleanup();
    bthread_mutex_destroy(&my_mutex);
    fprintf(stdout, "test PASSED\n");
}
