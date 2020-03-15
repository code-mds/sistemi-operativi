#include <stdio.h>
#include <setjmp.h>
#include <unistd.h>

static jmp_buf buf;

void dothings() {
    printf("Now I'm here\n");
    sleep(3);
    longjmp(buf, 42);

    printf("This is never printed\n"); // warning Unreachable code
}

int main(){
    if(!setjmp(buf)) {
        dothings();
    } else {
        printf("Now I'm there\n");
    }
    return 0;
}