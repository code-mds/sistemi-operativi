//
// Created by massi on 28.03.2020.
//
#include <stdio.h>
#include "test_bthread.h"
#include "test_tqueue.h"

int main() {
    // tqueue tests
    fprintf(stdout, "\nSTART tqueue tests\n");
    test_tqueue_null();
    test_tqueue_one_element();
    test_tqueue_many_elements();

    // bthread tests
    test_round_robin_scheduling();
    test_random_scheduling();
    test_priority_scheduling();
//    test_lottery_scheduling();
}