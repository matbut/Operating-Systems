#define _GNU_SOURCE
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>

void parse_args(int, char**);
void sent_signals();
void print_stats();

//Signal handlers
void set_signals_handlers(void(int, siginfo_t *, void *));
void child_handler(int, siginfo_t*, void*);
void parent_handler(int, siginfo_t*, void*);

pid_t child_pid;

volatile int signals_num;
volatile int type;
volatile int sent = 0;
volatile int received_by_child = 0;
volatile int received_by_parent = 0;

int ENDSIG;
int MESSIG;

int main(int argc, char** argv) {   
    parse_args(argc,argv);

    child_pid=fork();
    if(child_pid<0){
        perror("new process failed");
        exit(EXIT_FAILURE);
    }else if (child_pid==0){
        set_signals_handlers(child_handler);
        while (1);
    }else{
        set_signals_handlers(parent_handler);
        sleep(1);
        sent_signals();
        print_stats();
    }
    return 0;
} 

void sent_signals(){
    for(int i=0;i<signals_num;i++){
        kill(child_pid, MESSIG);
        sent++;
        if(type==2) pause();
    }
    kill(child_pid, ENDSIG);
    sent++;
    
    int status;
    waitpid(child_pid, &status, 0);
    if (WIFEXITED(status))
        received_by_child = WEXITSTATUS(status);
    else {
        perror("Child exit error\n");
        exit(EXIT_FAILURE);
    }
}

void set_signals_handlers(void (sa)(int, siginfo_t *, void *)){
    struct sigaction act;
    act.sa_flags = SA_SIGINFO | SA_RESTART;
    sigfillset(&act.sa_mask);
    act.sa_sigaction = sa;

    sigdelset(&act.sa_mask, MESSIG);
    sigdelset(&act.sa_mask, ENDSIG);
    if(sigaction(MESSIG,&act,NULL)<0){
        perror("MESSIG error");
        exit(EXIT_FAILURE);  
    }
    if(sigaction(ENDSIG,&act,NULL)<0){
        perror("ENDSIG error");
        exit(EXIT_FAILURE);  
    }
}

void child_handler(int sig_num, siginfo_t *info, void *context){
    if (sig_num == MESSIG) {
        received_by_child++;
        kill(getppid(), MESSIG);
    }else if (sig_num == ENDSIG) {
        received_by_child++;
        exit(received_by_child);
    }
}

void parent_handler(int sig_num, siginfo_t *info, void *context) {
    if (sig_num == SIGINT) {
        kill(child_pid, SIGKILL);
        exit(EXIT_FAILURE);
    }
    if (sig_num == MESSIG && info->si_pid == child_pid) {
        received_by_parent++;
    } 
}

void parse_args(int argc, char** argv){
    if (argc != 3) {
        printf("Ilegal arguments number");
        exit(EXIT_FAILURE);
    }
        
    signals_num = (int) strtol(argv[1], '\0', 10);
    type = (int) strtol(argv[2], '\0', 10);

    switch(type){
        case 1: case 2:
            MESSIG=SIGUSR1;
            ENDSIG=SIGUSR2;
            break;
        case 3:
            MESSIG=SIGRTMAX;
            ENDSIG=SIGRTMIN;
            break;     
        default:
            printf("Ilegal type argument: %d",type);
            exit(EXIT_FAILURE);
    }
}

void print_stats() {
    printf("-- Signals --\n");
    printf("Sent: %d\n", sent);
    printf("Received by child: %d\n", received_by_child);
    printf("Received by parent: %d\n", received_by_parent);
}
