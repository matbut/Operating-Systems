#define _GNU_SOURCE
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include <getopt.h>

#define CREATE_CHILD 1 << 0
#define END_CHILD 1 << 1 
#define SEND_SIGNAL 1 << 2 
#define GET_SIGUSR1 1 << 3 
#define GET_SIGRT 1 << 4

#define WRITE(option, options,format, ...) if(options & option) { \
    char buffer[255]; \
    sprintf(buffer, format, ##__VA_ARGS__); \
    write(1, buffer, strlen(buffer)); }

#define ERROR(code, format, ...) { fprintf(stderr, format, ##__VA_ARGS__); exit(code);}

void parse_args(int, char**);

void set_signals_handlers();
//Signal handlers
void sigchld_handler(int , siginfo_t *, void *);
void deafault_real_time_handler(int , siginfo_t *, void *);
void sigusr1_handler(int , siginfo_t *, void *);
void sigint_handler(int , siginfo_t *, void *);

//Childs functions
int create_child();
void delete_child(pid_t);
void inform_child(pid_t,int);
void print_childs();

char CHILD_PROG[] = "./child";

int options=0;

int waiting_childs_num=0;
int max_waiting_childs_num=0;
int childs_num=0;
int max_childs_num=0;

pid_t *waiting_childs_pids;
pid_t *childs_pids;

int main(int argc, char** argv) {   

    parse_args(argc,argv);
    set_signals_handlers();

    childs_pids = calloc(max_childs_num, sizeof(pid_t));

    for(childs_num=0;childs_num<max_childs_num;childs_num++){
        childs_pids[childs_num]=create_child();
    }
    waiting_childs_pids = calloc(max_waiting_childs_num, sizeof(pid_t));

    while (wait(NULL)){
        if (errno == ECHILD){
            print_childs();
            printf("Those children disapeared!");
            exit(EXIT_SUCCESS);
        } 
    }
    return 0;
} 

void set_signals_handlers(){

    struct sigaction act;
    act.sa_flags = SA_SIGINFO | SA_RESTART;
    sigemptyset(&act.sa_mask);

    for (int i = SIGRTMIN; i <= SIGRTMAX; i++) {
        act.sa_sigaction = deafault_real_time_handler;
        if(sigaction(i,&act,NULL)<0){
            perror("Real Time signal error");
            exit(EXIT_FAILURE);  
        }
    }
   
    act.sa_sigaction = sigint_handler;
    if(sigaction(SIGINT,&act,NULL)<0){
        perror("SIGINT error");
        exit(EXIT_FAILURE);  
    }

    act.sa_sigaction = sigusr1_handler;
    if(sigaction(SIGUSR1,&act,NULL)<0){
        perror("SIGUSR1 error");
        exit(EXIT_FAILURE);  
    }

    act.sa_flags |= SA_NOCLDSTOP;
    act.sa_sigaction = sigchld_handler;
    if(sigaction(SIGCHLD,&act,NULL)<0){
        perror("SIGCHLD error");
        exit(EXIT_FAILURE);  
    }
}

void print_childs(){
    WRITE(1,1,"\nChilds:");
    for(int i=0;i<childs_num;i++)
        WRITE(1,1,"%d,",childs_pids[i]);        
    WRITE(1,1,"\n");
}

void sigchld_handler(int signum, siginfo_t *info, void *context) {
    delete_child(info->si_pid);
    if (info->si_code == CLD_EXITED) {
        WRITE(END_CHILD,options,"Child %d exited with status: %d\n", info->si_pid, info->si_status);
    } else {
        WRITE(END_CHILD,options,"Child %d killed by signal %d\n", info->si_pid, info->si_status);
    }
}

void deafault_real_time_handler(int sign_no, siginfo_t *info, void *context) {
    WRITE(GET_SIGRT,options,"Got RT %d form Child %d\n",sign_no, info->si_pid);
}

void sigusr1_handler(int sign_no, siginfo_t *info, void *context) {
        
    WRITE(GET_SIGUSR1,options,"Got SIGUSR1 form Child %d\n", info->si_pid);
    if (waiting_childs_num >= max_waiting_childs_num) {
        inform_child(info->si_pid,SIGUSR1);
    }else {
        waiting_childs_pids[waiting_childs_num] = info->si_pid;
        waiting_childs_num++;

        if (max_waiting_childs_num == waiting_childs_num) {
            for (int i=0; i<waiting_childs_num; i++) {
                inform_child(waiting_childs_pids[i],SIGUSR1);
            }
        }
    }
}

void sigint_handler(int sign_no, siginfo_t *info, void *context) {
    //WRITE(GET_SIGNAL,options,"Got SIGINIT form PID: %d\n", info->si_pid);
    for (int i = 0; i < max_childs_num; i++) {
        inform_child(childs_pids[i],SIGKILL);
    }
    exit(EXIT_SUCCESS);
}

void inform_child(pid_t child_pid,int sign_no){
    WRITE(SEND_SIGNAL,options,"Sending %d to Child %d\n",sign_no,child_pid);
    if(kill(child_pid,0) >= 0){
        kill(child_pid,sign_no);
    }else
        WRITE(SEND_SIGNAL,options,"Child %d does not exists\n", child_pid);
}

void delete_child(pid_t pid){
    int i;
    for(i=0;i<childs_num;i++)
        if(childs_pids[i]==pid)
            break;
    if(i==childs_num){
        WRITE(1,1,"Lost Child %d ",pid);
        exit(EXIT_FAILURE);
    }

    childs_pids[i]=childs_pids[childs_num-1];
    childs_num--;
}

int create_child(){
    pid_t child_pid=fork();
    if(child_pid<0){
        perror("new process failed");
        exit(EXIT_FAILURE);
    }else if (child_pid==0){
        if(execl(CHILD_PROG,CHILD_PROG, NULL)){
            perror("execute child prog failed");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }else
        WRITE(CREATE_CHILD,options,"Created new Child %d\n",child_pid);
    return child_pid;
}

void parse_args(int argc, char** argv){
    int c;
    while (1){    
        static struct option long_options[] ={
            /* These options don’t set a flag.
                We distinguish them by their indices. */
            {"processesNum",    required_argument,  0, 'p'},
            {"waitingNum",      required_argument,  0, 'w'},
            {"create",          no_argument,        0, 'c'},
            {"gotUSR1",         no_argument,        0, 'u'},
            {"gotRT",           no_argument,        0, 'r'},
            {"sent",            no_argument,        0, 's'},
            {"exit",            no_argument,        0, 'e'},
            {0, 0, 0, 0}
        };
    /* getopt_long stores the option index here. */
    int option_index = 0;

    c = getopt_long (argc, argv, "p:w:curse",long_options, &option_index);

    /* Detect the end of the options. */
    if (c == -1)
        break;

    switch(c){
        case 0:
            break;
        case 'p':
            max_childs_num = (int) strtol(optarg, '\0', 10);
            break;
        case 'w':
            max_waiting_childs_num = (int) strtol(optarg, '\0', 10);
            break;
        case 'c':
            options |= CREATE_CHILD;
            break;
        case 'u':
            options |=  GET_SIGUSR1;
            break;
        case 'r':
            options |=  GET_SIGRT;
            break;
        case 's':
            options |=  SEND_SIGNAL;
            break;
        case 'e':
            options |=  END_CHILD;
            break;                                    
        case '?':
            /* getopt_long already printed an error message. */
            break;
        default:
            printf("illegal argument: ");
            exit(EXIT_FAILURE);
        }
    }
    if (optind < argc){
        char error_message[]="non-option ARGV-elements: ";
        while (optind < argc)
            strcat(error_message,argv[optind++]);
            strcat(error_message," ");
        strcat(error_message,"\n");
        perror(error_message);
        exit(EXIT_FAILURE);
    } 
}