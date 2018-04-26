# define _GNU_SOURCE
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include "message.h"

int server_mqid,my_mqid;

FILE*file;

void send_message(int type,char* format,...){

    struct message message;
    message.message_text.pid=getpid();
    message.message_type=type;

    va_list ap; 
    va_start(ap,format);
    vsprintf(message.message_text.buf,format,ap);
    
    printf("Client %d sends %s \"%s\" to server\n",getpid(),TO_STRING(message.message_type),message.message_text.buf);

    if (msgsnd (server_mqid, &message, sizeof (struct message_text), 0) == -1) {
        perror ("client msgsnd error");
        exit (EXIT_FAILURE);
    }
}

void remove_queue(void){
    printf("Client %d removes queue\n",getpid());

    if (msgctl (my_mqid, IPC_RMID, NULL) == -1) {
            perror ("client msgctl error");
            exit (EXIT_FAILURE);
    }
}

void client_exit(void){
    send_message(STOP,"");
    remove_queue();
}

void receive_message(){

    struct message message;

    if (msgrcv (my_mqid, &message, sizeof (struct message_text), 0, 0) == -1) {
        perror ("client msgrcv error");
        exit (EXIT_FAILURE);
    }

    printf ("Client %d received %s \"%s\" from server\n\n",getpid(),TO_STRING(message.message_type),message.message_text.buf);  
}

int read_message_type(){
    char buffer[BUFFER_SIZE];

    if(file==stdin)
        printf ("Please type a message type: ");

    if(fgets (buffer, BUFFER_SIZE-2, file) != NULL){

        int length = strlen (buffer);
        if (buffer[length-1] == '\n')
            buffer[length-1] = '\0';

        if(strcmp(buffer, "MIRROR") == 0)
            return MIRROR;
        
        if(strcmp(buffer, "CALC") == 0)
            return CALC;
        
        if(strcmp(buffer, "TIME") == 0)
            return TIME;

        if(strcmp(buffer, "END") == 0)
            return END;
        printf("Illegal message type: %s\n",buffer);
    }
    exit(EXIT_SUCCESS);
}

void read_message(char*buffer){
    if(file==stdin)
        printf ("Please type a message: ");
    if(fgets (buffer, BUFFER_SIZE-2, file) != NULL){
        int length = strlen (buffer);
        if (buffer[length-1] == '\n')
            buffer[length-1] = '\0';
    }else{
        perror("Client fgets error");
        exit (EXIT_FAILURE);
    }
}

void sigint_handler(int sig_no) {
    exit(sig_no);
}

void sigterm_handler(int sig_no) {
    remove_queue();
    _exit(sig_no);
}

int main (int argc, char **argv){

    if (signal(SIGINT,sigint_handler) == SIG_ERR){
        perror("SIGINT error");
        exit(EXIT_FAILURE);
    }

    if (signal(SIGTERM,sigterm_handler) == SIG_ERR){
        perror("SIGTERM error");
        exit(EXIT_FAILURE);
    }

    file=stdin;
    if (argc==2) {
        file = fopen(argv[1], "r");
        if(file==NULL){
            perror("fopen error"); 
            exit(EXIT_FAILURE);
        }
    }else if(argc!=1){
        printf("Ilegal arguments number");
        exit(EXIT_FAILURE);
    }
    

    key_t server_queue_key;
 
    if ((my_mqid = msgget (IPC_PRIVATE, QUEUE_PERMISSIONS)) == -1) {
        perror ("client msgget error");
        exit (EXIT_FAILURE);
    }

    char* path = getenv("HOME");
    if(path == NULL){
        perror ("server getenv \"HOME\" error");
        exit (EXIT_FAILURE);  
    }

    if ((server_queue_key = ftok (path, PROJECT_ID)) == -1) {
        perror ("client ftok error");
        exit (EXIT_FAILURE);
    }

    if ((server_mqid = msgget (server_queue_key, 0)) == -1) {
        perror ("client msgget error");
        exit (EXIT_FAILURE);
    }

    if(atexit(client_exit) == -1){
        perror ("client atexit registration error!");
        exit (EXIT_FAILURE);        
    } 

    printf("Client %d\n", getpid());

    send_message(REGISTER,"%d",my_mqid);
    receive_message();

    char buffer[BUFFER_SIZE];
    while (1) {

        int type=read_message_type();
        strcpy(buffer,"");

        if(type == MIRROR || type == CALC)
            read_message(buffer);

        send_message(type,buffer);

        if(type == END || type == STOP)
            exit (EXIT_SUCCESS);

        receive_message();
    }
}








