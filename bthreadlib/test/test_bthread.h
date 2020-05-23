//
// Created by massi on 28.03.2020.
//

#ifndef TEST_BTHREAD_H
#define TEST_BTHREAD_H

    #include "../bthread.h"

    void test_random_scheduling();
    void test_round_robin_scheduling();
    void test_priority_scheduling();
    void test_lottery_scheduling();

    void start_test(bthread_scheduling_policy schedulingPolicy);
    void test_bthread_simple();
    void test_bthread_sleep();
    void test_bthread_cancel();
    void test_bthread_yield();

#endif //TEST_BTHREAD_H
