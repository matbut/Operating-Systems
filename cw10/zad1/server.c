#define _XOPEN_SOURCE 700
#define _BSD_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>
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
client clients[CLIENT_MAX];
pthread_mutex_t clients_mutex;

void init();
void clean_up();
void sigint_handler(int signo);

void *connection(void* args);
void *terminal(void* args);
void *ping(void* args);

void handle_connection(int client_fd);
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
void put_client(int fd,char* name,int name_size){
    if(clients_num==CLIENT_MAX)
        MYERR("Too much clients");
    clients[clients_num].fd=fd;
    clients[clients_num].name=malloc(name_size*sizeof(char));
    strcpy(clients[clients_num].name,name);
    clients[clients_num].active=1;
    printf("Register client %s\n",clients[clients_num].name);
    clients_num++;
}
int get_client_idx(int client_fd){
    int i=0;
    while(i<clients_num && clients[i].fd!=client_fd)
        i++;
    if(i==clients_num)
        return -1;
    return i;
}
void print_clients(){
    for(int i=0;i<clients_num;i++){
        printf("%d: %s %d\n",i,clients[i].name,clients[i].fd);
    }
}
void remove_client(int client_fd){
    int idx=get_client_idx(client_fd);
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


void send_message(int client_fd, message_t msg_type, char* content){
    void *buff;
    message_info msg_i;
    msg_i.msg_type = msg_type;
    msg_i.msg_size = strlen(content) + 1;

    if(msg_type==CALC)
        msg_i.msg_num = ++tasks_num;
    else
        msg_i.msg_num=0;

    buff = malloc(sizeof(message_info) + msg_i.msg_size);
    memcpy(buff, &msg_i, sizeof(message_info));
    memcpy(buff+sizeof(message_info), content, msg_i.msg_size);
    if (write(client_fd, buff, sizeof(message_info) + msg_i.msg_size) < 0)
        ERR("Send message error");
    
    //int idx=get_client_idx(client_fd);
    //printf("Send message %d type: %s content: %s\n",
    //msg_i.msg_num,TO_STRING(msg_i.msg_type),content);
}
void confirm_active(int client_fd){
    int idx=get_client_idx(client_fd);
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
			if(events[i].data.fd == sock_unix || events[i].data.fd == sock_inet){
				handle_connection(events[i].data.fd);
			} else {
				handle_message(events[i].data.fd);
			}
		}
    }	
}

void handle_connection(int server_fd){
	int client_fd;
	if ((client_fd = accept(server_fd, NULL, NULL)) < 0) return;
	
	message_info msg_i;
    char content[CONTENT_MAX];
    read(client_fd, &msg_i, sizeof(message_info));
    int content_size=msg_i.msg_size;
    read(client_fd, content,content_size);

    //printf("Recived message %d type: %s content: %s\n",
    //msg_i.msg_num,TO_STRING(msg_i.msg_type),content);

	pthread_mutex_lock(&clients_mutex);
	
	//check whether client can be registered
	if(is_full_clients()) {
        pthread_mutex_unlock(&clients_mutex);
        send_message(client_fd, SERVER_REJECT, "Registration failed.\nMaximum number of clients has been reached");
        shutdown(client_fd,SHUT_RDWR);
        close(client_fd);
        return;
    } 
    if (is_name_clients(content)){
        pthread_mutex_unlock(&clients_mutex);
        send_message(client_fd, SERVER_REJECT, "Registration failed.\nGiven client name already in use");
        shutdown(client_fd,SHUT_RDWR);
        close(client_fd);
        return;
    }
	
    put_client(client_fd,content,content_size);

	//send confirmation message
    send_message(client_fd, SERVER_OK, "Registration success");

	//register in epoll
	struct epoll_event event;
	event.events = EPOLLIN | EPOLLET;
	event.data.fd = client_fd;
	if (epoll_ctl(epoll, EPOLL_CTL_ADD, client_fd, &event) < 0) ERR("Epoll_ctl client failed")

    pthread_mutex_unlock(&clients_mutex);
}
void handle_message(int client_fd){
	message_info msg_i;
    char content[CONTENT_MAX];
    read(client_fd, &msg_i, sizeof(message_info));
    int content_size= msg_i.msg_size;
    read(client_fd, content,content_size);


    //printf("Recived message %d type: %s content: %s\n",
    //msg_i.msg_num,TO_STRING(msg_i.msg_type),content);
    

    switch(msg_i.msg_type) {
        case PONG: {
            //mutex is locked
            pthread_mutex_lock(&clients_mutex);
            confirm_active(client_fd);
            pthread_mutex_unlock(&clients_mutex);
            break;
        }
        case RESULT: {		
            pthread_mutex_lock(&clients_mutex);
            printf("Received from client %s task[%d] result: %s\n",clients[get_client_idx(client_fd)].name,msg_i.msg_num,content);
            pthread_mutex_unlock(&clients_mutex);
            break;
        }
        case UNREGISTER: {;
            pthread_mutex_lock(&clients_mutex);
            remove_client(client_fd);
            close(client_fd);
            shutdown(client_fd,SHUT_RDWR);
            pthread_mutex_unlock(&clients_mutex);
            break;
        }
        default:
            break;
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
            send_message(clients[idx].fd, CALC,line);
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
            send_message(clients[i].fd, PING, "Ping");
        }
        pthread_mutex_unlock(&clients_mutex);

        //memcpy(&b, &a, sizeof(a));
        sleep(1);

        pthread_mutex_lock(&clients_mutex);
        //remove clients
        for(int i=0;i<clients_num;i++){
            if(clients[i].active == 0){
                remove_client(clients[i].fd);
            }
        }
        pthread_mutex_unlock(&clients_mutex);
        sleep(1);
    }
	return (void *) 0;
}


void configure_unix_socket(){
	//create socket	
	if ((sock_unix = socket(AF_LOCAL,SOCK_STREAM,0)) == -1) ERR("Socket unix failed")

    struct sockaddr_un addr_unix;
    memset(&addr_unix, 0, sizeof(addr_unix));
    addr_unix.sun_family = AF_UNIX;
    strcpy(addr_unix.sun_path, sock_path_unix);

    if(bind(sock_unix, (struct sockaddr *)&addr_unix, sizeof(struct sockaddr)) == -1) ERR("Bind unix failed")

    if(listen(sock_unix, CLIENT_MAX) == -1) ERR("Listen unix failed")
}

void configure_inet_socket(){
	//create socket
	if ((sock_inet = socket(AF_INET,SOCK_STREAM,0)) == -1) ERR("Socket inet failed")

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
 
    if(listen(sock_inet, CLIENT_MAX) == -1) ERR("Listen inet failed")
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
		if(shutdown(sock_inet, SHUT_RDWR ) == -1) fprintf(stderr,"Shutdown inet failed\n");
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
        send_message(clients[i].fd,SERVER_ERR,"Server exit");
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

