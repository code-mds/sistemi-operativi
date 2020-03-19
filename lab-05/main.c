#include <stdio.h>
#include <setjmp.h>
#include <unistd.h>
#include <stdlib.h>

#define CUSHION_SIZE 10000
#define save_context(CONTEXT) setjmp(CONTEXT)
#define restore_context(CONTEXT) longjmp(CONTEXT, 1)

typedef enum {
    __BTHREAD_UNINITIALIZED,
    __BTHREAD_READY,
    __BTHREAD_COMPLETED
} bthread_state;

typedef void *(*bthread_routine) (void *);

typedef struct {
    bthread_state state;
    bthread_routine routine;
    jmp_buf buf;
} thread_t;

void yield_next_thread(int thread_idx);
thread_t *get_next_thread();

void* bthread0(void* arg);
void* bthread1(void* arg);
void* bthread2(void* arg);
void* bthread3(void* arg);

thread_t _threads[] = {
        { __BTHREAD_UNINITIALIZED, bthread0, },
        { __BTHREAD_UNINITIALIZED, bthread1,  },
        { __BTHREAD_UNINITIALIZED, bthread2,  },
        { __BTHREAD_UNINITIALIZED, bthread3,  },
};

const int NR_LOOP = 10; //10000;
int _threads_count = sizeof(_threads) / sizeof(_threads[0]);
int _current_thread = 0;

void create_cushion_and_call(thread_t *thread)
{
    char cushion[CUSHION_SIZE];
    cushion[CUSHION_SIZE-1] = cushion[0];
    thread->state = __BTHREAD_READY;
    thread->routine(NULL);
}

void yield_next_thread(int thread_idx) {
    if (save_context(_threads[thread_idx].buf))
        return;

    thread_t * next_thread = get_next_thread();
    if(next_thread == NULL) {
        // fix: *** stack smashing detected ***: <unknown> terminated
        // return;

        // no more threads to run, restore the initiator thread
        restore_context(_threads[0].buf);
    }

    if (next_thread->state == __BTHREAD_UNINITIALIZED) {
        create_cushion_and_call(next_thread);
    } else {
        restore_context(next_thread->buf);
    }
}

thread_t * get_next_thread() {
    for (int i = _current_thread+1; i < _threads_count + _current_thread; ++i) {
        int next = i % _threads_count;
        if(_threads[next].state != __BTHREAD_COMPLETED) {
            _current_thread = next;
            return &_threads[next];
        }
    }
    return NULL;
}

void* bthread0(void* arg) {
    for(int i=0;i<NR_LOOP;i++) {
        printf("BThread0, i=%d\n", i);
        yield_next_thread(0);
    }
    _threads[0].state = __BTHREAD_COMPLETED;
    printf("BThread0 completed\n");
    yield_next_thread(0);
}

void* bthread1(void* arg) {
    for(int i=0;i<NR_LOOP;i++) {
        printf("BThread1, i=%d\n", i);
        yield_next_thread(1);
    }
    _threads[1].state = __BTHREAD_COMPLETED;
    printf("BThread1 completed\n");
    yield_next_thread(1);
}

void* bthread2(void* arg) {
    for(int i=0;i<5;i++) {
        printf("BThread2, i=%d\n", i);
        yield_next_thread(2);
    }
    _threads[2].state = __BTHREAD_COMPLETED;
    printf("BThread2 completed\n");
    yield_next_thread(2);
}

void* bthread3(void* arg) {
    for(int i=0;i<NR_LOOP;i++) {
        printf("BThread3, i=%d\n", i);
        yield_next_thread(3);
    }
    _threads[3].state = __BTHREAD_COMPLETED;
    printf("BThread3 completed\n");
    yield_next_thread(3);
}

void main() {
    create_cushion_and_call(&_threads[0]);
}