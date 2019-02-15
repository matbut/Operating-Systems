#define _XOPEN_SOURCE 700
#define _BSD_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <endian.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include "header.h"

#define ERR(str) {perror(str); exit(EXIT_FAILURE);}
#define MYERR(str) {printf(str); exit(EXIT_FAILURE);}

//local and inet socket representation
int sock_unix=-1; 
int sock_inet=-1; 
int epoll = -1;

uint16_t tcp_port_num; //TCP port number
char* sock_path_unix; // unix socket path

pthread_t connection_thread;
pthread_t terminal_thread;
pthread_t ping_thread;

int clients_num=0;
int client_idx=0;
int tasks_num=0; 
int new_client_id=1;
client clients[CLIENT_MAX];
pthread_mutex_t clients_mutex;

void init();
void clean_up();
void sigint_handler(int signo);

void *connection(void* args);
void *terminal(void* args);
void *ping(void* args);

void handle_connection(message msg, struct sockaddr *client_addr,int size,int sock);
void handle_message(int fd);

void doubt_active(int idx);
void confirm_active(int client_fd);


int is_full_clients(){
    return clients_num>=CLIENT_MAX;
}
int is_empty_clients(){
    return clients_num==0;
}
int is_name_clients(char* name){
    int i=0;
    while(i<clients_num && strcmp(clients[i].name,name)!=0)
        i++;
    if(i==clients_num)
        return 0;
    return 1;
}
int put_client(char* name,int name_size,struct sockaddr*client_addr,int size,int sock){
    if(clients_num==CLIENT_MAX)
        MYERR("Too much clients");
    clients[clients_num].addr=malloc(size);
    clients[clients_num].addr_size=size;
    clients[clients_num].sock=sock;
    memcpy(clients[clients_num].addr,client_addr,size);
    clients[clients_num].id=new_client_id++;
    clients[clients_num].name=malloc(name_size*sizeof(char));
    strcpy(clients[clients_num].name,name);
    clients[clients_num].active=1;
    printf("Register client %s\n",clients[clients_num].name);
    clients_num++;
    return clients_num-1;
}
int get_client_idx(int client_id){
    int i=0;
    while(i<clients_num && clients[i].id!=client_id)
        i++;
    if(i==clients_num)
        return -1;
    return i;
}
void print_clients(){
    for(int i=0;i<clients_num;i++){
        printf("%d: %s %d\n",i,clients[i].name,clients[i].id);
    }
}
void remove_client(int client_id){
    int idx=get_client_idx(client_id);
    if(idx==-1) MYERR("Can't find client");
    printf("Unregister client %s\n",clients[idx].name);

    clients_num--;
    clients[idx]=clients[clients_num];
}
int next_client_idx(){
    client_idx++;
    if(client_idx>=clients_num) client_idx=0;
    return client_idx;
}


void send_message(int sock_fd, message_t msg_type, char* content,int msg_id,struct sockaddr*client_addr,int size){
    
    message msg;
    msg.msg_type = msg_type;
    msg.id=msg_id;
    strcpy(msg.content,content);
    if(msg_type==CALC)
        msg.msg_num = ++tasks_num;
    else
        msg.msg_num=0;

    if(sendto(sock_fd,&msg,sizeof(message),0,client_addr,(socklen_t) size)<0)
        ERR("Send message error");
}
void confirm_active(int client_id){
    int idx=get_client_idx(client_id);
    if(idx==-1) MYERR("Can't find client");
    clients[idx].active=1;
}
int main(int argc, char **argv){
	if(argc != 3)
        MYERR("Incorrect number of arguments\n");

    if(signal(SIGINT,sigint_handler)==SIG_ERR) ERR("signal SIGINT error");
    
	if(atexit(clean_up)==-1) ERR("atexit error");

	tcp_port_num = htons((uint16_t) atoi(argv[1]));
	sock_path_unix = argv[2];

    init();

    printf("Hi, I am server\n");

    sigset_t set;
    int sig;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    pthread_sigmask(SIG_BLOCK, &set, NULL);
    
    if(pthread_create(&connection_thread,  NULL, connection, NULL)==-1) ERR("connection pthread_create error");
    if(pthread_create(&terminal_thread,  NULL, terminal, NULL)==-1) ERR("connection pthread_create error");
    if(pthread_create(&ping_thread,  NULL, ping, NULL)==-1) ERR("connection pthread_create error");
    
    sigwait(&set, &sig);    
    exit(EXIT_SUCCESS);
}
void *connection(void* args){
    struct epoll_event events[CLIENT_MAX];
    int ev_num;
    while (1) {		
		if ((ev_num = epoll_wait(epoll, events, CLIENT_MAX, -1)) == -1) ERR("Epoll_wait failed") 
		for(int i=0; i< ev_num; i++){
			handle_message(events[i].data.fd);
		}
    }	
}

void handle_connection(message msg, struct sockaddr *client_addr,int size,int sock){	

	pthread_mutex_lock(&clients_mutex);
	
	//check whether client can be registered
	if(is_full_clients()) {
        pthread_mutex_unlock(&clients_mutex);
        send_message(sock,SERVER_REJECT,"Registration failed.\nMaximum number of clients has been reached",
        0,client_addr,size);
        return;
    } 
    if (is_name_clients(msg.content)){
        pthread_mutex_unlock(&clients_mutex);
        send_message(sock, SERVER_REJECT, "Registration failed.\nGiven client name already in use",
        0,client_addr,size);
        return;
    }
	
    int client_idx=put_client(msg.content,strlen(msg.content),client_addr,size,sock);
	//send confirmation message
    send_message(sock, SERVER_OK,"Registration succes",clients[client_idx].id,client_addr,size);
    pthread_mutex_unlock(&clients_mutex);
}
void handle_message(int sock_fd){

    int size;
    if(sock_fd == sock_inet) 
        size=sizeof(struct sockaddr_in);
    else
        size=sizeof(struct sockaddr_un);
    struct sockaddr*client_addr=malloc(size);
    message msg;

    if(recvfrom(sock_fd,&msg,sizeof(message),0,client_addr,(socklen_t *)&size)!=-1){
        switch(msg.msg_type) {
            case REGISTER: {
                handle_connection(msg,client_addr,size,sock_fd);
                break;
            }
            case PONG: {
                pthread_mutex_lock(&clients_mutex);
                confirm_active(msg.id);
                pthread_mutex_unlock(&clients_mutex);
                break;
            }
            case RESULT: {		
                pthread_mutex_lock(&clients_mutex);
                printf("Received from client %s task[%d] result: %s\n",clients[get_client_idx(msg.id)].name,msg.msg_num,msg.content);
                pthread_mutex_unlock(&clients_mutex);
                break;
            }
            case UNREGISTER: {;
                pthread_mutex_lock(&clients_mutex);
                remove_client(msg.id);
                pthread_mutex_unlock(&clients_mutex);
                break;
            }
            default:
                break;
        }
    }
}

void *terminal(void* args){
	char* line = NULL;
    size_t len = 0;
    while(1) {
        if(getline(&line, &len, stdin)==-1) ERR("getline error");
        line[len-1]='\0';
        pthread_mutex_lock(&clients_mutex);
        if(is_empty_clients())
            printf("No clients to response to handle that operation\n"); 
        else{
            int idx=next_client_idx();
            printf("Send to client %s task[%d]: %s",clients[idx].name,tasks_num,line);
            send_message(clients[idx].sock,CALC,line,0,clients[idx].addr,clients[idx].addr_size);
        }
        pthread_mutex_unlock(&clients_mutex);
    }
}

void *ping(void* args){
    while(1){
        //dubt active clients
        pthread_mutex_lock(&clients_mutex);
        for(int i=0;i<clients_num;i++){
            clients[i].active = 0;
            send_message(clients[i].sock,PING,"Ping",0,clients[i].addr,clients[i].addr_size);
        }
        pthread_mutex_unlock(&clients_mutex);

        //memcpy(&b, &a, sizeof(a));
        sleep(1);

        pthread_mutex_lock(&clients_mutex);
        //remove clients
        for(int i=0;i<clients_num;i++){
            if(clients[i].active == 0){
                remove_client(clients[i].id);
            }
        }
        pthread_mutex_unlock(&clients_mutex);
        sleep(1);
    }
	return (void *) 0;
}


void configure_unix_socket(){
	//create socket	
	if ((sock_unix = socket(AF_LOCAL,SOCK_DGRAM,0)) == -1) ERR("Socket unix failed")

    struct sockaddr_un addr_unix;
    memset(&addr_unix, 0, sizeof(addr_unix));
    addr_unix.sun_family = AF_UNIX;
    strcpy(addr_unix.sun_path, sock_path_unix);

    if(bind(sock_unix, (struct sockaddr *)&addr_unix, sizeof(struct sockaddr)) == -1) ERR("Bind unix failed")
}

void configure_inet_socket(){
	//create socket
	if ((sock_inet = socket(AF_INET,SOCK_DGRAM,0)) == -1) ERR("Socket inet failed")

	int flag=1;
	if (setsockopt(sock_inet, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) < 0) ERR("Setsockopt failed")
	
	//prepare address structures
    struct in_addr addr; 
    memset(&addr, 0, sizeof(addr));
    addr.s_addr = INADDR_ANY; //dowolny adres IP

	struct sockaddr_in addr_inet;
	memset(&addr_inet, 0, sizeof(addr_inet));
	addr_inet.sin_family = AF_INET;
	addr_inet.sin_port = tcp_port_num;
	addr_inet.sin_addr = addr; 
	
    if(bind(sock_inet, (struct sockaddr *)&addr_inet, sizeof(struct sockaddr)) == -1) ERR("Bind inet failed")
}

void configure_epol(){
	if ((epoll = epoll_create1(0)) == -1) ERR("Epoll_create1 failed");
	struct epoll_event event;
	event.events = EPOLLIN;

	//register sockets
	event.data.fd = sock_unix;
	if(epoll_ctl(epoll, EPOLL_CTL_ADD, sock_unix, &event) == -1) ERR("Epoll_ctl unix failed")
	event.data.fd = sock_inet;
	if(epoll_ctl(epoll, EPOLL_CTL_ADD, sock_inet, &event) == -1) ERR("Epoll_ctl inet failed")
}

void init(){
	configure_unix_socket();
	configure_inet_socket();
    configure_epol();
}

void clean_up_unix(){
	if(sock_unix != -1) {
		if(shutdown(sock_unix, SHUT_RDWR ) == -1) fprintf(stderr,"Shutdown unix failed\n");
		if(close(sock_unix) == -1) fprintf(stderr,"Close unix failed\n");
		if(unlink(sock_path_unix) == -1) fprintf(stderr,"Unlink failed\n");
	}
}
void clean_up_inet(){
	if(sock_inet != -1) {
		if(close(sock_inet) == -1) fprintf(stderr,"Close inet failed\n");
	}
}
void clean_up_epoll(){
	if(epoll != -1){
		if (close(epoll) == -1) fprintf(stderr,"Close epoll failed\n");
	}
}

void clean_up(){
    pthread_mutex_lock(&clients_mutex);
    for(int i=0;i<clients_num;i++) 
        send_message(clients[i].sock,SERVER_ERR,"Server exit",0,clients[i].addr,clients[i].addr_size);
    pthread_mutex_unlock(&clients_mutex);  
    clean_up_unix();
    clean_up_inet();
    clean_up_epoll();
}

void sigint_handler(int signo){
    pthread_cancel(ping_thread);
    pthread_cancel(terminal_thread);
    pthread_cancel(connection_thread);
	exit(EXIT_FAILURE);
}

