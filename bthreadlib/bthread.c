//
// Created by massi on 22.03.2020.
//
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <sys/time.h>
#include <stdbool.h>
#include <signal.h>
#include "bthread.h"
#include "bthread_private.h"

#define STACK_SIZE  100000
#define save_context(CONTEXT) sigsetjmp(CONTEXT, 1)
#define restore_context(CONTEXT) siglongjmp(CONTEXT, 1)

// 100 ms
#define QUANTUM_USEC 1000

double get_current_time_millis()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000 ;
}

static void bthread_setup_timer() {
    static bool initialized = false;

    if (!initialized) {
        signal(SIGALRM, (void (*)()) bthread_yield);
        //signal(SIGVTALRM, (void (*)()) bthread_yield);
        struct itimerval time;
        time.it_interval.tv_sec = 0;
        time.it_interval.tv_usec = QUANTUM_USEC;
        time.it_value.tv_sec = 0;
        time.it_value.tv_usec = QUANTUM_USEC;
        initialized = true;
//        setitimer(ITIMER_VIRTUAL, &time, NULL);
        setitimer(ITIMER_REAL, &time, NULL);
    }
}

void bthread_block_timer_signal(){
    sigset_t signalset;
    sigemptyset(&signalset);
    sigaddset(&signalset, SIGVTALRM);
    sigprocmask(SIG_BLOCK, &signalset, NULL);
}

void bthread_unblock_timer_signal() {
    sigset_t signalset;
    sigemptyset(&signalset);
    sigaddset(&signalset, SIGVTALRM);
    sigprocmask(SIG_UNBLOCK, &signalset, NULL);
}

/*
 * Returns a "view" on the queue beginning at the node containing data for the thread identified by
 * bthread. If the queue is empty or doesn't contain the corresponding data this function returns NULL.
 */
static TQueue bthread_get_queue_at(bthread_t bthread) {
    volatile __bthread_scheduler_private* scheduler = bthread_get_scheduler();
    TQueue node = scheduler->current_item;
    __bthread_private* tp;
    do {
        tp = (__bthread_private*) tqueue_get_data(node);
        // empty queue
        if(tp == NULL)
            return NULL;

        if(tp->tid == bthread)
            return node;

        node = tqueue_at_offset(node, 1);
    } while (node != scheduler->current_item);

    // bthread not found
    return NULL;
}

/*
 * Checks whether the thread referenced by the parameter bthread has reached a zombie state. If
 * it's not the case the function returns 0. Otherwise the following steps are performed: if retval is
 * not NULL the exit status of the target thread (i.e. the value that was supplied to bthread_exit)
 * is copied into the location pointed to by *retval; the thread's stack is freed and the thread's
 * private data structure is removed from the queue (Note: depending on your implementation, you
 * might need to pay attention to the special case where the scheduler's queue pointer itself
 * changes!); finally the function returns 1.
 */
static int bthread_check_if_zombie(bthread_t bthread, void **retval) {
    TQueue view = bthread_get_queue_at(bthread);
    int status = 0;
    if(view != NULL) {
        __bthread_private *tp = (__bthread_private *) tqueue_get_data(view);
        if (tp->state == __BTHREAD_ZOMBIE) {
            if (retval != NULL)
                *retval = tp->retval;

            tp->state = __BTHREAD_EXIT;
            if(tp->stack) {
                free(tp->stack);
                tp->stack = NULL;
            }

            status = 1;
        } else if(tp->state == __BTHREAD_EXIT)
            status = 1;
    }

    return status;
}

/*
 * clean queue for unit test
 */
void bthread_cleanup() {
    //TODO svuotare la coda
}

/*
 * This private function creates, maintains and returns a static pointer to the singleton instance of
 * __bthread_scheduler_private. Fields of this structure need to be initialized as NULL.
 * Other functions will call this method to obtain this pointer. This function should not be accessible
 * outside the library.
 */
__bthread_scheduler_private *bthread_get_scheduler() {
    static __bthread_scheduler_private * scheduler = NULL;
    if(scheduler == NULL) {
        scheduler = malloc(sizeof(__bthread_scheduler_private));
        scheduler->current_item = NULL;
        scheduler->queue = NULL;
        scheduler->current_tid = 0;
    }

    return scheduler;
}

/*
 * Creates a new thread structure and puts it at the end of the queue. The thread identifier (stored
 * in the buffer pointed by bthread) corresponds to the position in the queue. The thread is not
 * started when calling this function. Attributes passed through the attr argument are ignored
 * (thus it is possible to pass a NULL pointer). The stack pointer for new created threads is NULL.
 */
int bthread_create(bthread_t *bthread, const bthread_attr_t *attr, void *(*start_routine)(void *), void *arg) {
    __bthread_private* thread = malloc(sizeof(__bthread_private));
    thread->tid = tqueue_enqueue(&bthread_get_scheduler()->queue, thread);
    *bthread = thread->tid;
    thread->body = start_routine;
    thread->state = __BTHREAD_READY;
    thread->stack = NULL;
    thread->wake_up_time = 0;
    thread->cancel_req = 0;
//    thread->attr = *attr;
    thread->arg = arg;

    return 0;
}

/*
 *  Waits for the thread specified by bthread to terminate (i.e. __BTHREAD_ZOMBIE state), by
 *  scheduling all the threads. In the following we will discuss some details about this procedure.
 */
int bthread_join(bthread_t bthread, void **retval) {
    bthread_setup_timer();

    volatile __bthread_scheduler_private* scheduler = bthread_get_scheduler();
    scheduler->current_item = scheduler->queue;

    save_context(scheduler->context);

    bthread_block_timer_signal();
    if (bthread_check_if_zombie(bthread, retval)) {
        bthread_unblock_timer_signal();
        return 0;
    }

    __bthread_private* tp;
    do {
        // update current_item with the next item
        scheduler->current_item = tqueue_at_offset(scheduler->current_item, 1);
        tp = (__bthread_private*) tqueue_get_data(scheduler->current_item);
//        bthread_printf( "JOIN: tid: %d  state: %d\n", tp->tid, tp->state);
        // check sleeping threads
        double now = get_current_time_millis();
        if(tp->state == __BTHREAD_SLEEPING && now > tp->wake_up_time)
            tp->state = __BTHREAD_READY;

    } while (tp->state != __BTHREAD_READY);


    if (tp->stack) {
        bthread_unblock_timer_signal();
        // stack already initialized
        restore_context(tp->context);
    } else {
        // setup a new stack (allocating memory on the heap)
        tp->stack = (char*) malloc(sizeof(char) * STACK_SIZE);
        unsigned long target = tp->stack + STACK_SIZE - 1;
        #if __APPLE__
            // OSX requires 16 bytes aligned stack
            target -= (target % 16);
        #endif

        // change the stack pointer register
        #if __x86_64__
            asm __volatile__("movq %0, %%rsp" :: "r"((intptr_t) target));
        #else
            asm __volatile__("movl %0, %%esp" :: "r"((intptr_t) target));
        #endif

        bthread_unblock_timer_signal();
        // call the thread's routine (tp->body: void* f(void*) )
        bthread_exit(tp->body(tp->arg));
    }
    bthread_unblock_timer_signal();
    return 0;
}

/*
 * Saves the thread context and restores (long-jumps to) the scheduler context. Saving the thread
 * context is achieved using sigsetjmp, which is similar to setjmp but can also save the signal
 * mask if the provided additional parameter is not zero (to restore both the context and the signal
 * mask the corresponding call is siglongjmp). Saving and restoring the signal mask is required
 * for implementing preemption.
 */
void bthread_yield() {
    bthread_block_timer_signal();

    volatile __bthread_scheduler_private* scheduler = bthread_get_scheduler();
    // save current thread context: sigsetjmp
    __bthread_private* tp = (__bthread_private*) tqueue_get_data(scheduler->current_item);
    if (!save_context(tp->context)) {
        bthread_printf( "YIELD: tid: %lu  state: %d\n", tp->tid, tp->state);
        // restore scheduler context: siglongjmp
        restore_context(scheduler->context);
    }

    bthread_unblock_timer_signal();
}

/*
 * Terminates the calling thread and returns a value via retval that will be available to another
 * thread in the same process that calls bthread_join, then yields to the scheduler. Between
 * bthread_exit and the corresponding bthread_join the thread stays in the __BTHREAD_ZOMBIE state.
 */
void bthread_exit(void *retval) {
    bthread_block_timer_signal();
    volatile __bthread_scheduler_private* scheduler = bthread_get_scheduler();
    __bthread_private* tp = (__bthread_private*) tqueue_get_data(scheduler->current_item);
    tp->retval = retval;
    tp->state = __BTHREAD_ZOMBIE;
    restore_context(scheduler->context);
}

/**
 * The ms parameter specifies the number of milliseconds the thread must sleep.
 */
void bthread_sleep(double ms) {
    volatile __bthread_scheduler_private* scheduler = bthread_get_scheduler();
    __bthread_private* tp = (__bthread_private*) tqueue_get_data(scheduler->current_item);
    tp->state = __BTHREAD_SLEEPING;
    tp->wake_up_time = get_current_time_millis() + ms;
    bthread_block_timer_signal();
    if (!save_context(tp->context)) {
        bthread_printf( "SLEEP: tid: %lu  state: %d\n", tp->tid, tp->state);
        // restore scheduler context: siglongjmp
        restore_context(scheduler->context);
    }
    bthread_unblock_timer_signal();
}

void bthread_cancel(bthread_t bthread) {
    TQueue view = bthread_get_queue_at(bthread);
    if(view != NULL) {
        __bthread_private *tp = (__bthread_private *) tqueue_get_data(view);
        tp->cancel_req = 1;
        bthread_printf( "bthread_cancel: tid: %lu  state: %d\n", tp->tid, tp->state);
    }
}

void bthread_testcancel() {
    volatile __bthread_scheduler_private* scheduler = bthread_get_scheduler();
    __bthread_private* tp = (__bthread_private*) tqueue_get_data(scheduler->current_item);
    if(tp->cancel_req) {
        bthread_printf( "bthread_testcancel: tid: %lu  state: %d\n", tp->tid, tp->state);
        bthread_exit((void *) -1);
    }
}

void bthread_printf(const char* format, ...) // requires stdlib.h and stdarg.h
{
    bthread_block_timer_signal();
    va_list args;
    va_start (args, format);
    vprintf (format, args);
    va_end (args);
    bthread_unblock_timer_signal();
}