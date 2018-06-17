#define _XOPEN_SOURCE 700
#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <math.h>
#include <errno.h>
#include <unistd.h>
#include <sys/times.h>
#include <time.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <endian.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "header.h"

#define ERR(str) {perror(str); exit(EXIT_FAILURE);}
#define MYERR(str) {printf(str); exit(EXIT_FAILURE);}

char* client_name;
int connection_type;
struct sockaddr* server_addr = NULL;
int server_fd = -1;
int task_num=0;

//auxiliary variable
char result[CONTENT_MAX];

void client_routine();
void calculate(char *operation);
void init(int argc, char **argv);
void register_to_server();
void send_message(message_t msg_type, char* content);
void sigint_handler(int signo);
void clean_up();

int main (int argc, char **argv){

    if(signal(SIGINT,sigint_handler)==SIG_ERR) ERR("signal SIGINT error");
	if(atexit(clean_up)==-1) ERR("atexit error");

    init(argc,argv);

    printf("Hi, I am client %s\n",client_name);

    client_routine();
    
    exit(EXIT_SUCCESS);
}

void client_routine(){
	char content[CONTENT_MAX];
    message_info msg_i;

    while(1){
        read(server_fd, &msg_i, sizeof(message_info));
        read(server_fd, content, msg_i.msg_size);
        task_num = msg_i.msg_num;
        switch(msg_i.msg_type){
            case PING: {
                send_message(PONG, "Pong");
                break;
            }
            case CALC: {
                printf("Client %s Received task[%d]: %s",client_name,task_num,content);
                calculate(content);
                printf("Client %s Send task[%d] result: %s\n",client_name,task_num,result);
                send_message(RESULT, result);
                break;
            }
            case SERVER_ERR: {
                printf("Client %s exit due to server error\n",client_name);
				exit(EXIT_FAILURE);
			}   
            default:
                printf("Client %s got illegal message\n",client_name);
                break;            
        }
	}
}

void calculate(char *buffer){
    int a,b;
    char operation;
    if(sscanf(buffer, "%d %c %d", &a,&operation,&b) < 0)
        strcpy(result,"can't scanf calculator expresion");
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
            strcpy(result,"illegal calculator operation");
            return;
    }
    sprintf(result,"%d",a);
}

void init(int argc, char **argv){
	if(argc != 4 && argc != 5) MYERR("Incorrect number of arguments\n");
    client_name = argv[1];
    server_addr = malloc(sizeof(struct sockaddr));
    if(strcmp(argv[2],"INET") == 0){
		//set connection type
		connection_type = AF_INET;
		
		//prepare server address		
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(atoi(argv[4]));
			struct in_addr sin_addr; 
			memset(&sin_addr, 0, sizeof(sin_addr));
			if(inet_pton(AF_INET, argv[3], &sin_addr) < 0) ERR("Inet_pton failed")
		addr.sin_addr = sin_addr; 
        server_addr = (struct sockaddr*) &addr;
        //------------
        if ((server_fd = socket(connection_type, SOCK_STREAM, 0)) < 0) ERR("Socket failed")
        if (connect(server_fd, (struct sockaddr*) &addr, sizeof(struct sockaddr_in)) < 0) ERR("Connect failed")
        
	} else if(strcmp(argv[2],"UNIX") == 0 || strcmp(argv[2],"LOCAL") == 0){
		//set connection type
		connection_type = AF_UNIX;
		
		//prepare server address	
		struct sockaddr_un addr;
		memset(&addr, 0, sizeof(addr));
		addr.sun_family = AF_UNIX;
		strcpy(addr.sun_path, argv[3]);
        server_addr = (struct sockaddr*) &addr;

        if ((server_fd = socket(connection_type, SOCK_STREAM, 0)) < 0) ERR("Socket failed")
        if (connect(server_fd, (struct sockaddr*) &addr, sizeof(struct sockaddr_un)) < 0) ERR("Connect failed")
	} else MYERR("Not supported connection type\n");
	register_to_server();
}

void register_to_server(){    
	printf("Sending registration message to server\n");
	send_message(REGISTER, client_name);
	
	message_info msg_i;
    char content[CONTENT_MAX];
    
    read(server_fd, &msg_i, sizeof(message_info));
    read(server_fd, content, msg_i.msg_size);

    if (msg_i.msg_type == SERVER_OK){
        printf("%s\n", content);
    }else{
        if(msg_i.msg_type == SERVER_REJECT)
            printf("%s",content);
        MYERR("\n");
    } 


}

void send_message(message_t msg_type, char* content){
    void *buff;
    message_info msg_i;
    msg_i.msg_type = msg_type;
    msg_i.msg_size = strlen(content) + 1;
    msg_i.msg_num = task_num;

    buff = malloc(sizeof(message_info) + msg_i.msg_size);
    memcpy(buff, &msg_i, sizeof(message_info));
    memcpy(buff+sizeof(message_info), content, msg_i.msg_size);
    
    if (write(server_fd, buff, sizeof(message_info) + msg_i.msg_size) < 0) return; // add error detection?
    return;
}

void sigint_handler(int signo){
	exit(EXIT_FAILURE);
}

void clean_up(){
	if(server_fd != -1){
		send_message(UNREGISTER, client_name);
		shutdown(server_fd, SHUT_RDWR);
	}
    if(server_fd != -1) close(server_fd);
}

