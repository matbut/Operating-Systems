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
    
    printf("Client %d[%d] sends %s to [%d] server\n",(int)getpid(),my_mqid,message.message_text.buf,server_mqid);
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
    printf ("Message received from server: %s\n\n", message.message_text.buf);  
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

    send_message(REGISTER,"%d",my_mqid);
    receive_message();


    printf ("Please type a message: ");
    char buffer[BUFFER_SIZE];
    while (fgets (buffer, BUFFER_SIZE-2, stdin)) {
        // remove newline from string
        int length = strlen (buffer);
        if (buffer[length-1] == '\n')
           buffer[length-1] = '\0';

        send_message(MIRROR,buffer);
        receive_message();

        printf ("Please type a message: ");
    }

    exit (EXIT_SUCCESS);
}








