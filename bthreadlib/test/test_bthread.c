//
// Created by massi on 28.03.2020.
//
#include <stdio.h>
#include <stdlib.h>
#include "../bthread.h"

void* my_routine(void* param) {
    int p = (int)param;
    fprintf(stdout, "thread %d  START \n", p);

    for (int i = 0; i < 5; ++i) {
        fprintf(stdout, "thread %d  -> %d\n", p, i);
        bthread_yield();
    }

    return (void*)p;
}

void test_bthread_create() {
    fprintf(stdout, "** test_bthread_create **\n");

    bthread_t tid[2];
    for (int i = 0; i < 2; ++i) {
        bthread_create(&tid[i], NULL, my_routine, (void*)i);
        fprintf(stdout, "%i) thread %d created\n", i, tid[i]);
    }

    for (int i = 0; i < 2; ++i) {
        int retval = -1;
        fprintf(stdout, "bthread_join: %d\n", tid[i]);
        bthread_join(tid[i], (void**)&retval);
    }

    fprintf(stdout, ": PASSED\n");
}

