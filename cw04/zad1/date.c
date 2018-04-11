#define _XOPEN_SOURCE
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>

char SHELL_SCRIPT[] = "./date_print.sh";
pid_t child_pid=-1;

void sigint_handler(int);
void sigtstp_handler(int);
void create_child();

int main(int argc, char** argv) {   

    if (signal(SIGINT,sigint_handler) == SIG_ERR){
        perror("SIGINT error");
        exit(EXIT_FAILURE);
    }

    struct sigaction act;
    act.sa_handler = sigtstp_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if(sigaction(SIGTSTP,&act,NULL)<0){
        perror("SIGTSTP error");
        exit(EXIT_FAILURE);  
    }

    create_child();
    while(1) 
        pause();
    return 0;
} 

void sigint_handler(int sig_no) {
    printf("\nOtrzymalem signal %d.\n", sig_no);


    if(kill(child_pid,0) >= 0){
        kill(child_pid,9);
        int st;
        
		if(waitpid(child_pid, &st, 0)<0){
            perror("Wait error");
            exit(EXIT_FAILURE);
        }
    }
    exit(EXIT_SUCCESS);
}

void sigtstp_handler(int sig_no) {

    if(kill(child_pid,0) >= 0){
        //exists
        printf("\nOtrzymalem signal %d.\nOczekuje na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu\n", sig_no);
        kill(child_pid,9);
        int st;

		if(waitpid(child_pid, &st, 0)<0){
            perror("Wait error");
            exit(EXIT_FAILURE);
        }

    }else{
        //not exists
        printf("\n");
        create_child();
    }
}

void create_child(){
    child_pid=fork();

    if(child_pid<0){
        perror("new process failed");
        exit(EXIT_FAILURE);
    }else if (child_pid==0){
        if(execl(SHELL_SCRIPT,SHELL_SCRIPT, NULL)){
            perror("execute shell script failed");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }
}
