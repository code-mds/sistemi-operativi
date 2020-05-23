//
// Created by massi on 23.05.2020.
//

#ifndef TEST_CRITICALSECTION_H
#define TEST_CRITICALSECTION_H

#include "../bthread.h"
#include "../tmutex.h"

void test_sum(bthread_scheduling_routine thread_proc);
void test_mutex();
void* thread_proc_mutex(void* arg);

void test_nomutex();
void* thread_proc_nomutex(void* arg);

#endif //TEST_CRITICALSECTION_H
