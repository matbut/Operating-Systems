#define _GNU_SOURCE
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>

void sigusr1_handler(int signum) {
    kill(getppid(), SIGRTMIN + (rand() % (SIGRTMAX - SIGRTMIN)));
}

int main() {
    srand(getpid());

    if (signal(SIGUSR1,sigusr1_handler) == SIG_ERR){
        perror("SIGINT error");
        exit(EXIT_FAILURE);
    }

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    //zablokowanie wszystkich sygnalow

    int sleep_time = 1+(rand() % 10);
    printf("Child PID %d: I am sleeping: %ds\n", getpid(), sleep_time);
    sleep((unsigned int) sleep_time);
    kill(getppid(), SIGUSR1);

    //przywrocenie starej maski sygnalow
    sigsuspend(&mask);

    return sleep_time;
}
