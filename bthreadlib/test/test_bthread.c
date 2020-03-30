//
// Created by massi on 28.03.2020.
//
#include <stdio.h>
#include <stdlib.h>
#include "../bthread.h"

void* my_routine(void* param) {
    int p = (int)param;

//    for (int i = 0; i < 5; ++i) {
//        bthread_yield();
//    }

    return (void*)p;
}

void test_bthread_create() {
    fprintf(stdout, "test_bthread_create ");

    bthread_t tid[2];
    for (int i = 0; i < 2; ++i) {
        bthread_create(&tid[i], NULL, my_routine, i);
    }

    for (int i = 0; i < 2; ++i) {
        int retval = -1;
        bthread_join(tid[i], (void**)&retval);
        if(retval != tid[i]) {
            fprintf(stderr, ": expected %i found %i FAILED\n", i, retval);
            return;
        }
    }

    fprintf(stdout, ": PASSED\n");
}

