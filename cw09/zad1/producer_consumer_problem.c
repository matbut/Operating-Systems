#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include <sys/times.h>
#include <pthread.h>
#include <unistd.h>
#include <limits.h>
#include <signal.h>
#include <stdarg.h>

int P; //producers
int K; //consumers
int N; //buffer size
char file_path[FILENAME_MAX];; //input file
FILE*fp;
int L; //lines
char search_type; 
int print_type; 
int nk; //program end

char **buffer;
pthread_mutex_t **buffer_mutex;
int consumption_index = 0, products_num = 0;
pthread_mutex_t *consumption_index_mutex,*products_num_mutex,*file_mutex;
pthread_cond_t *empty_buffer_cond;
pthread_cond_t *full_buffer_cond;

int finished = 0;
pthread_t *producer_threads;
pthread_t *consumer_threads;

#define print_producer(format, ...)  \
    printf("Producer[%ld]: ", pthread_self()); \
    printf(format, ##__VA_ARGS__); \
    printf("\n"); \
    fflush(stdout);

#define print_consumer(format, ...)  \
    printf("Consumer[%ld]: ", pthread_self()); \
    printf(format, ##__VA_ARGS__); \
    printf("\n"); \
    fflush(stdout);

int is_empty(){
    return products_num==0;
}
int is_full(){
    return products_num>=N;
}

void lock(pthread_mutex_t *mutex){
    if(pthread_mutex_lock(mutex)!=0){
        perror("pthread_mutex_lock");
        exit(EXIT_FAILURE);
    }
}
void unlock(pthread_mutex_t *cond){
    if(pthread_mutex_unlock(cond)!=0){
        perror("pthread_mutex_unlock");
        exit(EXIT_FAILURE);
    }
}
void broadcast(pthread_cond_t *mutex){
    if(pthread_cond_broadcast(mutex)!=0){
        perror("pthread_cond_broadcast");
        exit(EXIT_FAILURE);
    }
}

void put_string(char* string){
    lock(products_num_mutex);
    while(is_full()){
        if(print_type) {print_producer("is waiting for not full buffer");}
        pthread_cond_wait(full_buffer_cond,products_num_mutex);
    }

    lock(consumption_index_mutex);
    int idx=(consumption_index+products_num)%N;
    if(print_type) {print_producer("is in buffer[%d]",idx);}
    unlock(consumption_index_mutex);
    products_num++;
    lock(buffer_mutex[idx]);  
    unlock(products_num_mutex);
      
    buffer[idx] = malloc((strlen(string) + 1) * sizeof(char));
    strcpy(buffer[idx], string);

    if(print_type) {print_producer("put string to buffer[%d]",idx);}
    unlock(buffer_mutex[idx]);
    broadcast(empty_buffer_cond);
}

char* get_string(){
    char* string;
    lock(products_num_mutex);
    while(is_empty()){
        if(finished){
            if(print_type) {print_consumer("end consumption");}
             pthread_exit(NULL);
        }
        if(print_type) {print_consumer("is waiting for not empty buffer");}
        pthread_cond_wait(empty_buffer_cond,products_num_mutex);
    }
    lock(consumption_index_mutex);
    products_num--;
    unlock(products_num_mutex);

    lock(buffer_mutex[consumption_index]);
    if(print_type) {print_consumer("is in buffer[%d]",consumption_index);}
    string=buffer[consumption_index];
    buffer[consumption_index]=NULL;
    print_consumer("get string from buffer[%d]",consumption_index);
    unlock(buffer_mutex[consumption_index]);

    consumption_index++;
    if(consumption_index>=N){
        consumption_index=0;
    }   
    unlock(consumption_index_mutex);
    broadcast(full_buffer_cond);
    return string;
}

void end(){
    for(int i=0;i<N;i++){
        if(pthread_mutex_destroy(buffer_mutex[i])!=0){
            perror("pthread_mutex_destroy");
            exit(EXIT_FAILURE);
        }
    }
    if(pthread_mutex_destroy(consumption_index_mutex)!=0 ||
        pthread_mutex_destroy(products_num_mutex)!=0 ||
        pthread_mutex_destroy(file_mutex)!=0){
        perror("pthread_mutex_destroy");
        exit(EXIT_FAILURE);
    }    
    if(pthread_cond_destroy(empty_buffer_cond)!=0 ||
        pthread_cond_destroy(full_buffer_cond)!=0 ){
        perror("pthread_cond_destroy");
        exit(EXIT_FAILURE);  
    }  
     
    if(fclose(fp)!=0){
        perror(file_path);
        exit(EXIT_FAILURE);
    }
}

void thread_init(){
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    if(nk>0)
        sigaddset(&set, SIGALRM);

    if(pthread_sigmask(SIG_BLOCK, &set, NULL)!= 0){
        perror("pthread_sigmask");
        exit(EXIT_FAILURE);  
    }
}

void sig_hanlder(int signo) {
    printf("Recived signal\n"); fflush(stdout);
    exit(EXIT_SUCCESS);
}

void init(){
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = sig_hanlder;

    if(sigaction(SIGINT,&act,NULL)<0){
        perror("sigaction");
        exit(EXIT_FAILURE);  
    }
    if(nk>0){
        if(sigaction(SIGALRM,&act,NULL)<0){
            perror("sigaction");
            exit(EXIT_FAILURE);  
        }
        alarm(nk);
    }
    
    buffer = malloc(N * sizeof(char*));

    buffer_mutex = malloc(N * sizeof(pthread_mutex_t*));
    for(int i=0;i<N;i++){
        buffer_mutex[i]=malloc(sizeof(pthread_mutex_t));
        if(pthread_mutex_init(buffer_mutex[i], NULL)!=0 
        //||pthread_mutexattr_settype(buffer_mutex[i],PTHREAD_MUTEX_ERRORCHECK)
            ){   
            perror("pthread_mutex_init");
            exit(EXIT_FAILURE);  
        }
    }
    
    consumption_index_mutex=malloc(sizeof(pthread_mutex_t));
    products_num_mutex=malloc(sizeof(pthread_mutex_t));
    file_mutex=malloc(sizeof(pthread_mutex_t));
    if(pthread_mutex_init(consumption_index_mutex, NULL)!=0 ||
        pthread_mutex_init(products_num_mutex, NULL)!=0 ||
        pthread_mutex_init(file_mutex, NULL)!=0
        //|| pthread_mutexattr_settype(consumption_index_mutex,PTHREAD_MUTEX_ERRORCHECK) 
        //|| pthread_mutexattr_settype(products_num_mutex,PTHREAD_MUTEX_ERRORCHECK)
        ){   
        perror("pthread_mutex_init");
        exit(EXIT_FAILURE);  
    }

    empty_buffer_cond=malloc(sizeof(pthread_cond_t));
    full_buffer_cond=malloc(sizeof(pthread_cond_t));
    if(pthread_cond_init(empty_buffer_cond,NULL)!=0 ||
        pthread_cond_init(full_buffer_cond,NULL)!=0){
        perror("pthread_cond_destroy");
        exit(EXIT_FAILURE);  
    }  

    fp = fopen(file_path, "r");

    if(fp==NULL){
        perror(file_path);
        exit(EXIT_FAILURE);
    }
    
}

void *producer(void *args) {
    thread_init();

    if(print_type) {print_producer("started production");}
    
    char * line = NULL;
    size_t len = 0;
    ssize_t read = 0;

    lock(file_mutex);
    while (((read = getline(&line, &len, fp)) != -1)) {
        strtok(line,"\n");
        if(print_type) {print_producer("read next line:  \"%s \"",line);}
        unlock(file_mutex);
        put_string(line);
        lock(file_mutex);
    }
    unlock(file_mutex);
    
    if(print_type) {print_producer("end production")};
    finished=1;
    broadcast(empty_buffer_cond);
    return NULL;
}
void config(char *config_path) {
    FILE *fp = fopen(config_path, "r");
    if(fp==NULL){
        perror(config_path);
        exit(EXIT_FAILURE);
    }
    fscanf(fp,"%d %d %d %s %d %c %d %d",&P,&K,&N,file_path,&L,&search_type,&print_type,&nk);


    if(fclose(fp)!=0){
        perror(config_path);
        exit(EXIT_FAILURE);
    }
}

int compare(char* string){

    switch (search_type){
        case '=':
            return strlen(string)==L;
        case '>':
            return strlen(string)>L;
        case '<':
            return strlen(string)<L;
    }
    return 0;
}

void *consumer(void *args) {
    thread_init();

    if(print_type) {print_consumer("started consumption");}

    char*string = get_string();
    while(string!=NULL){
        if(compare(string)){
            print_consumer("consume string \"%s\"",string);
        }
        if(string) free(string);
        string = get_string();
    }
    return NULL;
}


void printHelp(char *progName){
  printf("\nUsage help\n");
  printf("%s <config>  \n",progName);
}


void start_threads() {
    producer_threads=malloc(P*sizeof(pthread_t));
    for (int p = 0; p < P; ++p)
        if(pthread_create(&producer_threads[p], NULL, producer, NULL)!=0){
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    consumer_threads=malloc(K*sizeof(pthread_t));
    for (int k = 0; k < K; ++k)
        if(pthread_create(&consumer_threads[k], NULL, consumer, NULL)!=0){
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    if (nk > 0) alarm(nk);
}

void join_threads(){
    for (int p = 0; p < P; ++p)
        if(pthread_join(producer_threads[p], NULL)!=0){
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }

    for (int k = 0; k < K; ++k)
        if(pthread_join(consumer_threads[k],NULL)!=0){
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }
}

int main (int argc, char **argv){


    if (argc!=2) {
        printf("Ilegal arguments number");
        printHelp(argv[0]);
        exit(EXIT_FAILURE);
    }
    config(argv[1]);
    init();
    start_threads();
    join_threads();
    end();
    return EXIT_SUCCESS;
}


