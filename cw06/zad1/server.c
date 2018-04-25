# define _GNU_SOURCE
#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include "message.h"

int qid;

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
    if (msgctl (qid, IPC_RMID, NULL) == -1) {
        perror ("server msgctl error");
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
    
    printf("Server sends %s to client %d\n",message.message_text.buf,(int) pid);

    // send message to server
    if (msgsnd (client_mqid, &message, sizeof (struct message_text), 0) == -1) {
        perror ("client msgsnd error");
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
        perror("cant'scanf message queue identifier");
        exit (EXIT_FAILURE);
    } 

    if(clients_num > CLIENTS_MAX_NUM-1){
        printf("Maximum amount of clients reached!\n");
    }else{
        
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
    response(pid,REGISTER,"%d",a);
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

void process_init(pid_t pid,char *buffer){

}

void receive(){

    struct message message;

    int msgflg=0;
    if(end_flag){
        msgflg=IPC_NOWAIT;
    }

    if (msgrcv (qid, &message, sizeof (struct message_text), 0, msgflg) == -1) {
        if(end_flag){
            printf("Server ends...");
            exit (EXIT_SUCCESS); //no bo skonczyly sie wiadomosci, a jest flaga IPC_NOWAIT
        }
        perror ("server msgrcv error");
        exit (EXIT_FAILURE);
    }

    printf("Server receives %s from client %d\n",message.message_text.buf,message.message_text.pid);

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
        case INIT:
            process_init(message.message_text.pid,message.message_text.buf);
            break;    
        default:
            printf ("unknown message type");
            exit (EXIT_FAILURE);      
    }
}

int main (int argc, char **argv){

    if(atexit(remove_queue) == -1){
        perror ("server atexit registration error!");
        exit (EXIT_FAILURE);        
    } 

    char* path = getenv("HOME");
    if(path == NULL){
        perror ("server getenv \"HOME\" error");
        exit (EXIT_FAILURE);  
    }

    key_t msg_queue_key;
    if ((msg_queue_key = ftok (path, PROJECT_ID)) == -1) {
        perror ("server ftok error");
        exit (EXIT_FAILURE);
    }

    if ((qid = msgget (msg_queue_key, IPC_CREAT | QUEUE_PERMISSIONS)) == -1) {
        perror ("server msgget error");
        exit (EXIT_FAILURE);
    }

    printf("Server %d\n", getpid());

    while (1) {
        // read an incoming message
        receive();
        //printf ("Server: response sent to client.\n");
    }
    exit (EXIT_SUCCESS);    
}