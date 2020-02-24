#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <ctype.h>

int main(){

    int x = 50;

    printf("Sono il PID %d\n", getpid());
    int s;

    pid_t c = fork();
    if (c == 0){
        printf("Sono il figlio, PID=%d, Parent=%d, %d\n", getpid(), getppid(), x);
        return 77;
        //in questo caso rimane il figlio ZOMBIE che ha terminato prima del padre il processo
    } else if (c>0){
        x = 70; //non condividono nulla
        printf("Sono il padre di PID=%d, %d\n", c, x);
        sleep(10);
        waitpid(c, &s, 0);
        printf("Figlio uscito con valore %d\n", WEXITSTATUS(s));
        sleep(60);
    } else{
        printf("Houston abbiamo un problema\n");
    }
}