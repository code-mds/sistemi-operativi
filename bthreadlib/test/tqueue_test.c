//
// Created by massi on 19.03.2020.
//

#include <stdlib.h>
#include <stdio.h>
#include "../tqueue.h"

typedef struct {
    char str[10];
    int len;
} data_t;

int test_data(data_t data[], long int data_sz);
void test_one_element();
void test_many_elements();

void test_null();

int main() {
    test_null();
    test_one_element();
    test_many_elements();
}

void test_null() {
    fprintf(stdout, "test_null ");
    tqueue_get_data(NULL);
    tqueue_pop(NULL);
    tqueue_size(NULL);
    tqueue_enqueue(NULL, NULL);
    tqueue_at_offset(NULL, 0);
    fprintf(stdout, "PASSED\n");
}

void test_many_elements() {
    fprintf(stdout, "test_many_elements ");
    data_t data[] = {
            {"ciao", 1},
            {"pippo", 2},
            {"urchi", 3}
    };
    long int data_sz = sizeof(data) / sizeof(data[0]);
    test_data(data, data_sz);
}

void test_one_element() {
    fprintf(stdout, "test_one_element ");
    data_t data[] = {"ciao", 10};
    long int data_sz = sizeof(data) / sizeof(data[0]);
    test_data(data, data_sz);
}

int test_data(data_t data[], long int data_sz) {
    TQueue q = NULL;
    for (int i = 0; i < data_sz; ++i) {
        tqueue_enqueue(&q, &data[i]);
    }

    unsigned long int queue_sz = tqueue_size(q);
    if (queue_sz != data_sz) {
        fprintf(stderr, "FAILED: wrong queue size. expected %ld actual %ld\n", data_sz, queue_sz);
        return (1);
    }

    for (int j = data_sz-1; j >= 0; --j) {
        void *val = tqueue_pop(&q);
        if (((data_t *) val) != &data[j]) {
            fprintf(stderr, "FAILED: wrong return data from pop\n");
            return (1);
        }
    }

    queue_sz = tqueue_size(q);
    if (queue_sz != 0) {
        fprintf(stderr, "FAILED: wrong queue size. expected 0 actual %ld\n", queue_sz);
        return (1);
    }

    fprintf(stdout, "PASSED\n");
    return 0;
}

