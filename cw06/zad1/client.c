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
#include "message.h"

int server_mqid,my_mqid;

void send_message(int type,char* format,...){


    struct message message;
    message.message_text.pid=getpid();
    message.message_type=type;

    va_list ap; 
    va_start(ap,format);
    vsprintf(message.message_text.buf,format,ap);
    
    printf("Client %d sends %s to server\n",(int)getpid(),message.message_text.buf);
    // send message to server
    if (msgsnd (server_mqid, &message, sizeof (struct message_text), 0) == -1) {
        perror ("client msgsnd error");
        exit (EXIT_FAILURE);
    }
}

void remove_queue(void){
    if (msgctl (my_mqid, IPC_RMID, NULL) == -1) {
            perror ("client msgctl error");
            exit (EXIT_FAILURE);
    }
}

void receive_message(){

    struct message message;

    // read response from server
    if (msgrcv (my_mqid, &message, sizeof (struct message_text), 0, 0) == -1) {
        perror ("client msgrcv error");
        exit (EXIT_FAILURE);
    }

    // process return message from server
    printf ("Client %d received %s from server\n\n",getpid(),message.message_text.buf);  
}

int read_message_type(){
    char buffer[BUFFER_SIZE];

    while(1){
        printf ("Please type a message type: ");
        if(fgets (buffer, BUFFER_SIZE-2, stdin) == NULL){
            perror("Server fgets error");
            exit (EXIT_FAILURE);
        }

        int length = strlen (buffer);
        if (buffer[length-1] == '\n')
            buffer[length-1] = '\0';


        if(strcmp(buffer, "mirror") == 0)
            return MIRROR;
        
        if(strcmp(buffer, "calc") == 0)
            return CALC;
        
        if(strcmp(buffer, "time") == 0)
            return TIME;

        if(strcmp(buffer, "end") == 0)
            return END;

        printf("Illegal message type\n");
    }
}

void read_message(char*buffer){
    printf ("Please type a message: ");
    if(fgets (buffer, BUFFER_SIZE-2, stdin) == NULL){
        perror("Server fgets error");
        exit (EXIT_FAILURE);
    }

    int length = strlen (buffer);
    if (buffer[length-1] == '\n')
        buffer[length-1] = '\0';
}

int main (int argc, char **argv){

    if(atexit(remove_queue) == -1){
        perror ("client atexit registration error!");
        exit (EXIT_FAILURE);        
    } 

    key_t server_queue_key;

    // create my client queue for receiving messages from server
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

        if(type == END)
            exit (EXIT_SUCCESS);

        receive_message();
    }
}








