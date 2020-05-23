//
// Created by massi on 28.03.2020.
//
#include <stdio.h>
#include "test_bthread.h"
#include "test_tqueue.h"
#include "test_criticalsection.h"

void test_bthread();
void test_tqueue();
void test_criticalsection();

int main() {
    test_tqueue();
    test_bthread();
    test_criticalsection();
}

void test_criticalsection() {
    fprintf(stdout, "\n*******************************\n");
    fprintf(stdout, "START critical section tests\n");
    fprintf(stdout, "*******************************\n");
    // critical section tests
    test_mutex();
}


void test_tqueue() {
    // tqueue tests
    fprintf(stdout, "\n*******************************\n");
    fprintf(stdout, "START tqueue tests\n");
    fprintf(stdout, "*******************************\n");
    test_tqueue_null();
    test_tqueue_one_element();
    test_tqueue_many_elements();
}

void test_bthread() {// bthread tests
    fprintf(stdout, "\n*******************************\n");
    fprintf(stdout, "START bthread tests\n");
    fprintf(stdout, "*******************************\n");
    test_round_robin_scheduling();
    test_random_scheduling();
    test_priority_scheduling();
    test_lottery_scheduling();
}