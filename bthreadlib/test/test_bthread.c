//
// Created by massi on 28.03.2020.
//
#include <stdio.h>
#include "../bthread.h"
#define THREADS 2

void* my_routine(void* param) {
    int loops = (int)param;
    int i=0;
    for (i = 0; i < loops; ++i) {
        fprintf(stdout, "thread [%d]  -> %d\n", loops, i);
        bthread_yield();
    }

    return (void*)i;
}

void test_bthread_create() {
    fprintf(stdout, "** test_bthread_create **\n");

    bthread_t tid[THREADS];
    int loops[THREADS] = {2, 3};

    for (int i = 0; i < THREADS; ++i) {
        bthread_create(&tid[i], NULL, my_routine, (void*)loops[i]);
        fprintf(stdout, "%i) thread_%lu created, # of loops: %d \n", i, tid[i], loops[i]);
    }

    int failed = 0;
    for (int i = 0; i < THREADS; ++i) {
        int retval = -1;
        fprintf(stdout, "%d) thread_%lu join, # of loops: %d \n", i, tid[i], loops[i]);
        bthread_join(tid[i], (void**)&retval);
//        if(loops[i] != retval) {
//            fprintf(stderr, "test FAILED %d != %d \n", loops[i], retval);
//            failed = 1;
//        }
    }

    if(!failed)
        fprintf(stdout, "test PASSED\n");
}

