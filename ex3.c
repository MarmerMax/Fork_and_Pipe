#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>

//for children
int cpid[5];
int j;
void sigCathcher() {
    signal(SIGINT, sigCathcher);
    printf("PID %d caught one\n", getpid());
    if (j > -1){
        kill(cpid[j], SIGINT);
    }
}

int main(){
    int i;
    int zombie;
    int status;
    int pid;
    signal(SIGINT, sigCathcher);
    for(i = 0; i < 5; i++){
        if((pid = fork()) == 0){
            printf("PID %d ready\n", getpid());
            j = i - 1;
            pause(); // wait
            exit(0);
        }
        else{
            cpid[i] = pid;
        }
    }


    sleep(2);
    //send signals to the first child
    kill(cpid[4], SIGINT);
    sleep(2);

    for(i = 0; i < 5; i++){
        zombie = wait(&status);
        printf("%d is dead\n", zombie);
    }
    exit(0);
}