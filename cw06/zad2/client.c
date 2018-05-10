# define _GNU_SOURCE
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <mqueue.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include "message.h"

mqd_t private_id,public_id;

char path[50];

FILE*file;

void send_message(int type,char* format,...){

    struct message message;

    message.message_text.pid=getpid();
    message.message_type=type;

    va_list ap; 
    va_start(ap,format);
    vsprintf(message.message_text.buf,format,ap);
    
    printf("Client %d sends %s \"%s\" to server\n",getpid(),TO_STRING(message.message_type),message.message_text.buf);

    if(mq_send(public_id, (char*) &message, sizeof(struct message), 1) == -1){
        perror("client mq_send error");
        exit (EXIT_FAILURE);
    } 

}

void remove_queue(void){
    printf("Client %d removes queue\n",getpid());

    if(mq_close(public_id) == -1){
        perror("client mq_close server's queue error");
        exit (EXIT_FAILURE);
    }

    if(mq_close(private_id) == -1){
        perror("client mq_close client's queue error");
        exit (EXIT_FAILURE);
    }

    if(mq_unlink(path) == -1){
        perror("client mq_unlink error");
        exit (EXIT_FAILURE);
    }

}

void client_exit(void){
    send_message(STOP,"");
    remove_queue();
}

void receive_message(){

    struct message message;

    if(mq_receive(private_id,(char*) &message, sizeof(struct message), NULL) == -1){
        perror("client mq_receive error");
        exit(EXIT_FAILURE);
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
        perror("client fgets error");
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
        perror("client SIGINT error");
        exit(EXIT_FAILURE);
    }

    if (signal(SIGTERM,sigterm_handler) == SIG_ERR){
        perror("client SIGTERM error");
        exit(EXIT_FAILURE);
    }

    file=stdin;
    if (argc==2) {
        file = fopen(argv[1], "r");
        if(file==NULL){
            perror("client fopen error"); 
            exit(EXIT_FAILURE);
        }
    }else if(argc!=1){
        printf("Ilegal arguments number");
        exit(EXIT_FAILURE);
    }
    

    public_id = mq_open(server_path, O_WRONLY);
    if(public_id == -1){
        perror ("client mq_open server's queue error!");
        exit (EXIT_FAILURE);  
    }

    struct mq_attr posixAttr;

    posixAttr.mq_maxmsg = QUEUE_MAX_SIZE;
    posixAttr.mq_msgsize = sizeof(struct message);

    sprintf(path, "/%d", getpid());

    private_id = mq_open(path, O_RDONLY | O_CREAT | O_EXCL, 0666, &posixAttr);
    if(private_id == -1){
        perror ("client mq_open error");
        exit (EXIT_FAILURE);  
    } 
    
    printf("client mqid %d \n",private_id);


    if(atexit(client_exit) == -1){
        perror ("client atexit registration error");
        exit (EXIT_FAILURE);        
    } 

    printf("New client %d\n", getpid());

    send_message(REGISTER,"%d","");
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








