//
// Created by massi on 28.03.2020.
//
#include <stdio.h>
#include <stdlib.h>
#include "../bthread.h"

const int RETURN_VAL = 400;

void* my_routine(void* param) {
    return (void*)RETURN_VAL;
}
void test_bthread_create() {
    fprintf(stdout, "test_bthread_create ");

    bthread_t tid[2];
    for (int i = 0; i < 2; ++i) {
        bthread_create(&tid[i], NULL, my_routine, NULL);
    }

    for (int i = 0; i < 2; ++i) {
        bthread_join(tid[i], NULL);
    }

    fprintf(stdout, ": PASSED\n");
}

