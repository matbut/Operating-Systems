#define _GNU_SOURCE
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>

char CHILD_PROG[] = "./child";

int create_child();
void inform_child(pid_t,int);
int is_child(pid_t);
void printHelp(char *);
void delete_child(pid_t);

int k=0;
int n=0;
int K;
int N;
    
pid_t *waiting_childs_pids;
pid_t *childs;

void print_childs(){
    printf("\nChilds:");
    for(int i=0;i<n;i++)
        printf("%d,",childs[i]);
    printf("\nWaiting childs:");
    for(int i=0;i<k;i++)
        printf("%d,",waiting_childs_pids[i]);        
    printf("\n");
}

void sigchld_handler(int signum, siginfo_t *info, void *context) {
    if (info->si_code == CLD_EXITED) {
        printf("Child %d has terminated, with exit status: %d\n", info->si_pid, info->si_status);
    } else {
        printf("Child %d has terminated by signal: %d\n", info->si_pid, info->si_status);
    }
    delete_child(info->si_pid);
    if (n == 0) {
        printf("No more children, Terminating\n");
        exit(0);
    }
   
}

void deafault_real_time_handler(int signum, siginfo_t *info, void *context) {
    printf("Got RT %d form PID  %d\n",signum, info->si_pid);
}

void sigusr1_handler(int signum, siginfo_t *info, void *context) {
    
    if (!is_child(info->si_pid)){
        return;
    }
        
    printf("Got SIGUSR1 form child PID  %d\n", info->si_pid);

    if (k >= K) {
        inform_child(info->si_pid,SIGUSR1);
    }else {
        waiting_childs_pids[k] = info->si_pid;
        k++;
        if (k == K) {
            for (int i = 0; i < k; i++) {
                inform_child(waiting_childs_pids[i],SIGUSR1);
            }
        }
    }
}

void sigint_handler(int signum, siginfo_t *info, void *context) {
    printf("Got SIGINIT form PID: %d\n", info->si_pid);
    for (int i = 0; i < n; i++) {
        inform_child(childs[i],SIGKILL);
    }
    exit(EXIT_SUCCESS);
}

int main(int argc, char** argv) {   

    if (argc!=3) {
        printf("Ilegal arguments number");
        printHelp(argv[0]);
        exit(EXIT_FAILURE);
    }

    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    
    sigemptyset(&act.sa_mask);

    for (int i = SIGRTMIN; i <= SIGRTMAX; i++) {
        act.sa_sigaction = deafault_real_time_handler;
        if(sigaction(i,&act,NULL)<0){
            perror("Real Time signal error");
            exit(EXIT_FAILURE);  
        }
    }

    act.sa_sigaction = sigusr1_handler;
    if(sigaction(SIGUSR1,&act,NULL)<0){
        perror("SIGUSR1 error");
        exit(EXIT_FAILURE);  
    }

    act.sa_sigaction = sigint_handler;
    if(sigaction(SIGINT,&act,NULL)<0){
        perror("SIGINT error");
        exit(EXIT_FAILURE);  
    }

    act.sa_sigaction = sigchld_handler;
    if(sigaction(SIGCHLD,&act,NULL)<0){
        perror("SIGCHLD error");
        exit(EXIT_FAILURE);  
    }



    N = (int) strtol(argv[1], '\0', 10);
    K = (int) strtol(argv[2], '\0', 10);

    childs = calloc(N, sizeof(pid_t));
    for(n=0;n<N;n++)
        childs[n]=create_child();
    waiting_childs_pids = calloc(K, sizeof(pid_t));

    while (wait(NULL))
        if (errno == ECHILD){
            printf("My childs disapeared!");
            exit(EXIT_FAILURE);
        } 
    

    return 0;
} 

void inform_child(pid_t child_pid,int sign_no){
    printf("Sending %d to Child PID: %d\n",sign_no,child_pid);
    if(kill(child_pid,0) >= 0){
        kill(child_pid,sign_no);
        int status;
		waitpid(child_pid, &status, 0);

    }else
        printf("Child %d does not exists\n", child_pid);
}


int is_child(pid_t pid){
    for(int i=0;i<n;i++)
        if(childs[i]==pid)
            return 1;
    return 0;
}

void delete_child(pid_t pid){
    int i;
    for(i=0;i<n;i++)
        if(childs[i]==pid)
            break;
        
    childs[i]=childs[n-1];
    n--;
}

int create_child(){
    pid_t child_pid=fork();

    if(child_pid<0){
        perror("new process failed");
        exit(EXIT_FAILURE);
    }else if (child_pid==0){
        printf("Created new Child %d",child_pid);
        if(execl(CHILD_PROG,CHILD_PROG, NULL)){
            perror("execute child prog failed");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }
    return child_pid;
}

void printHelp(char *progName){
  printf("Usage help\n");
  printf("%s <childs number> <number for permission> \n",progName);
}