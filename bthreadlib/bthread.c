//
// Created by massi on 22.03.2020.
//
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include "bthread.h"
#include "bthread_private.h"

#define STACK_SIZE  100000
#define save_context(CONTEXT) sigsetjmp(CONTEXT, 1)
#define restore_context(CONTEXT) siglongjmp(CONTEXT, 1)

static __bthread_scheduler_private * _scheduler = NULL;

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
    if(view == NULL || retval == NULL)
        return 1;

    volatile __bthread_scheduler_private* scheduler = bthread_get_scheduler();
    __bthread_private* tp = (__bthread_private*) tqueue_get_data(scheduler->current_item);
    if(tp->state == __BTHREAD_ZOMBIE) {

        if(retval != NULL)
            *retval = tp->retval;

        free(tp->stack);
        return 1;
    }

    return 0;
}

/*
 * free static resources
 */
void bthread_cleanup() {
    free(_scheduler);
    _scheduler = NULL;
}

/*
 * This private function creates, maintains and returns a static pointer to the singleton instance of
 * __bthread_scheduler_private. Fields of this structure need to be initialized as NULL.
 * Other functions will call this method to obtain this pointer. This function should not be accessible
 * outside the library.
 */
__bthread_scheduler_private *bthread_get_scheduler() {
    if(_scheduler == NULL)
        _scheduler = malloc(sizeof(__bthread_scheduler_private));

    return _scheduler;
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

    //TODO
    //thread->attr = *attr;
    //thread->arg =
    //thread->retval =

    return 0;
}

/*
 *  Waits for the thread specified by bthread to terminate (i.e. __BTHREAD_ZOMBIE state), by
 *  scheduling all the threads. In the following we will discuss some details about this procedure.
 */
int bthread_join(bthread_t bthread, void **retval) {
    volatile __bthread_scheduler_private* scheduler = bthread_get_scheduler();
    scheduler->current_item = scheduler->queue;
    save_context(scheduler->context);
    if (bthread_check_if_zombie(bthread, retval))
        return 0;

    __bthread_private* tp;
    do {
        scheduler->current_item = tqueue_at_offset(scheduler->current_item, 1);
        tp = (__bthread_private*) tqueue_get_data(scheduler->current_item);
    } while (tp->state != __BTHREAD_READY);

    if (tp->stack) {
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

        // call the thread's routine (tp->body: void* f(void*) )
        bthread_exit(tp->body(tp->arg));
    }
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
    volatile __bthread_scheduler_private* scheduler = bthread_get_scheduler();
    // save current thread context: sigsetjmp
    __bthread_private* tp = (__bthread_private*) tqueue_get_data(scheduler->current_item);
    save_context(tp->context);

    // restore scheduler context: siglongjmp
    restore_context(scheduler->context);
}

/*
 * Terminates the calling thread and returns a value via retval that will be available to another
 * thread in the same process that calls bthread_join, then yields to the scheduler. Between
 * bthread_exit and the corresponding bthread_join the thread stays in the __BTHREAD_ZOMBIE state.
 */
void bthread_exit(void *retval) {
    volatile __bthread_scheduler_private* scheduler = bthread_get_scheduler();
    __bthread_private* tp = (__bthread_private*) tqueue_get_data(scheduler->current_item);

    //TODO

    tp->state = __BTHREAD_ZOMBIE;
}

