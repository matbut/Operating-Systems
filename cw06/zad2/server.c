# define _GNU_SOURCE
#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <mqueue.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include "message.h"

mqd_t public_id;

pid_t clients_pid[CLIENTS_MAX_NUM];
int clients_mqid[CLIENTS_MAX_NUM]; // message queue identifiers
int clients_num=0;

int end_flag=0;

int get_client_mqid(pid_t pid){
    for(int i=0;i<clients_num;i++){
        if(clients_pid[i]==pid)
            return clients_mqid[i];
    }
    printf("Can't find client message queue identifiers");
    exit(EXIT_FAILURE);   
}

void remove_queue(void){

    printf("Server removes queue\n");

    if(mq_close(public_id) == -1){
        perror ("server mg_close error");
        exit (EXIT_FAILURE);
    }

    if(mq_unlink(server_path) == -1){
        perror ("server mq_unlink error");
        exit (EXIT_FAILURE);
    }
}

void response(pid_t pid,int type,char* format,...){

    struct message message;

    message.message_text.pid=getpid();
    message.message_type=type;

    va_list ap; 
    va_start(ap,format);
    vsprintf(message.message_text.buf,format,ap);

    int client_mqid = get_client_mqid(pid);

    printf("Server sends %s \"%s\" to client %d\n",TO_STRING(message.message_type),message.message_text.buf,(int) pid);

    if(mq_send(client_mqid, (char*) &message, sizeof(struct message), 1) == -1){
        perror("server mq_send error!");
        exit (EXIT_FAILURE);
    } 
}

char* reverse(char *base){
    int length = (int) strlen(base);
    if(base[length] == '\n') length--;
    for(int i=0; i < length / 2; i++) {
        char tmp = base[i];
        base[i] = base[length-i-1];
        base[length-i-1] = tmp;
    }
    return base;
}

void process_register(pid_t pid,char *buffer){
    int client_mqid;
    if(sscanf(buffer, "%d", &client_mqid) < 0){
        perror("server process_register sscanf error");
        exit (EXIT_FAILURE);
    } 

    if(clients_num > CLIENTS_MAX_NUM-1){
        printf("Maximum clients number reached\n");
        exit(EXIT_FAILURE);
    }else{

        char clientPath[15];
        sprintf(clientPath, "/%d", pid);

        int clientMQD = mq_open(clientPath, O_WRONLY);
        if(clientMQD == -1 ){
            perror ("server mq_open client queue error");
            exit (EXIT_FAILURE);  
        }

        clients_pid[clients_num]=pid;
        clients_mqid[clients_num] = client_mqid;
        clients_num++;
        response(pid,REGISTER,"%d",clients_num-1);
        
    }
}

void process_mirror(pid_t pid,char *buffer){
    response(pid,MIRROR,"%s",reverse(buffer));
}

void process_calc(pid_t pid,char *buffer){

    int a,b;
    char operation;

    if(sscanf(buffer, "%d %c %d", &a,&operation,&b) < 0){
        perror("can't scanf calculator expresion");
        exit (EXIT_FAILURE);
    } 

    switch(operation){
        case '+':
            a=a+b;
            break;
        case '-':
            a=a-b;
            break;
        case '*':
            a=a*b;
            break;
        case '/':
            a=a/b;
            break;                              
        default:
            perror("illegal calculator operation");
            exit (EXIT_FAILURE);
    }
    response(pid,CALC,"%d",a);
}

void process_time(pid_t pid,char *buffer){
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    response(pid,TIME,"%s",asctime (timeinfo));
}

void process_end(pid_t pid,char *buffer){
    end_flag=1;
}

void remove_client_mqid(pid_t pid){
    for(int i=0;i<clients_num;i++){
        if(clients_pid[i]==pid){
            clients_num--;
            clients_pid[i]=clients_pid[clients_num];
            clients_mqid[i]=clients_mqid[clients_num];
            return;
        }
            
    }
    printf("Can't find client message queue identifiers");
    exit(EXIT_FAILURE);  
}

void process_stop(pid_t pid,char *buffer){
    remove_client_mqid(pid);
}

void receive(){

    struct message message;

    struct mq_attr currentState;
    
    if(end_flag){
        if(mq_getattr(public_id, &currentState) == -1){
            perror("server mq_getattr error");
            exit(EXIT_FAILURE);
        }
        
        if(currentState.mq_curmsgs == 0){
            printf("Server ends...");
            exit(EXIT_SUCCESS);
        }
            
    }

    if(mq_receive(public_id,(char*) &message, sizeof(struct message), NULL) == -1){
        perror("server mq_receive error");
        exit(EXIT_FAILURE);
    } 

    printf("Server receives %s \"%s\" from client %d\n",TO_STRING(message.message_type),message.message_text.buf,message.message_text.pid);

    switch (message.message_type){
        case REGISTER:
            process_register(message.message_text.pid,message.message_text.buf);
            break;
        case MIRROR:
            process_mirror(message.message_text.pid,message.message_text.buf);
            break;
        case CALC:
            process_calc(message.message_text.pid,message.message_text.buf);
            break;
        case TIME:
            process_time(message.message_text.pid,message.message_text.buf);
            break;
        case END:
            process_end(message.message_text.pid,message.message_text.buf);
            break;
        case STOP:
            process_stop(message.message_text.pid,message.message_text.buf);
            break;              
        default:
            printf ("unknown message type");
            exit (EXIT_FAILURE);      
    }
}

void sever_exit(void){
    for(int i=0;i<clients_num;i++)
        kill(clients_pid[i],SIGTERM);
    remove_queue();
}

void sigint_handler(int sig_no) {
    exit(sig_no);
}

int main (int argc, char **argv){

    if (signal(SIGINT,sigint_handler) == SIG_ERR){
        perror("SIGINT error");
        exit(EXIT_FAILURE);
    }
    
    struct mq_attr posixAttr;

    posixAttr.mq_maxmsg = QUEUE_MAX_SIZE;
    posixAttr.mq_msgsize = sizeof(struct message);

    public_id = mq_open(server_path, O_RDONLY | O_CREAT | O_EXCL, 0666, &posixAttr);
    if(public_id == -1){
        perror ("server mq_open server's queue error");
        exit (EXIT_FAILURE);  
    }

    if(atexit(sever_exit) == -1){
        perror ("server atexit registration error");
        exit (EXIT_FAILURE);        
    } 

    printf("New server %d\n", getpid());

    while (1) {
        receive();
    }
    exit (EXIT_SUCCESS);    
}