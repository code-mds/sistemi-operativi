#include <setjmp.h>
#include <stdio.h>

#define CUSHION_SIZE 10000

typedef enum { __BTHREAD_UNINITIALIZED, __BTHREAD_READY } bthread_state;
typedef void *(*bthread_routine) (void *);

void create_cushion_and_call(bthread_routine fn, bthread_state* state);
void* bthread1(void* arg);
void* bthread2(void* arg);
void* bthread3(void* arg);

jmp_buf bthread1_buf, bthread2_buf, bthread3_buf;
bthread_state bthread1_state = __BTHREAD_UNINITIALIZED;
bthread_state bthread2_state = __BTHREAD_UNINITIALIZED;
bthread_state bthread3_state = __BTHREAD_UNINITIALIZED;

void create_cushion_and_call(bthread_routine fn, bthread_state* state)
{
    char cushion[CUSHION_SIZE];
    cushion[CUSHION_SIZE-1] = cushion[0];
    *state = __BTHREAD_READY;
    fn(NULL);
}

void* bthread1(void* arg)
{
    int i;
    for(i=0;i<10000;i++) {
        printf("BThread1, i=%d\n", i);        
        /* Yield to next bthread */
        if (!setjmp(bthread1_buf)) {
            if (bthread2_state == __BTHREAD_UNINITIALIZED) {
                create_cushion_and_call(bthread2, &bthread2_state);
            } else {
                longjmp(bthread2_buf, 1);
            }
        }
    }
}

void* bthread2(void* arg)
{
    int i;
    for(i=0;i<10000;i++) {
        printf("BThread2, i=%d\n", i);        
        /* Yield to next bthread */
        if (!setjmp(bthread2_buf)) {
            if (bthread3_state == __BTHREAD_UNINITIALIZED) {
                create_cushion_and_call(bthread3, &bthread3_state);
            } else {
                longjmp(bthread3_buf, 1);
            }
        }
    }
}

void* bthread3(void* arg)
{
    int i;
    for(i=0;i<10000;i++) {
        printf("BThread3, i=%d\n", i);        
        /* Yield to next bthread */
        if (!setjmp(bthread3_buf)) {
            // We assume that bthread1 is already initialized
            longjmp(bthread1_buf, 1);
        }
    }
}

void main()
{
    create_cushion_and_call(bthread1, &bthread1_state);
}




