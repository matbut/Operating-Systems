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


//TODO
//pthread_cond_broadcast

int P; //producers
int K; //consumers
int N; //buffer size
char file_path[FILENAME_MAX];; //input file
int L; //lines
int search_type; 
int print_type; 
int nk; //program end

char **buffer;
pthread_mutex_t **buffer_mutex;
int consumption_index = 0, products_num = 0;
pthread_mutex_t *consumption_index_mutex,*products_num_mutex;
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
void put_string(char* string){

    pthread_mutex_lock(products_num_mutex);
    while(is_full()){
        print_producer("is waiting for not full buffer");
        pthread_cond_wait(full_buffer_cond,products_num_mutex);
    }

    pthread_mutex_lock(consumption_index_mutex);
    int idx=(consumption_index+products_num)%N;
    print_producer("is in buffer[%d]",idx);
    pthread_mutex_unlock(consumption_index_mutex);
    products_num++;

    pthread_mutex_lock(buffer_mutex[idx]);  
    pthread_mutex_unlock(products_num_mutex);
      
    buffer[idx]=string;
    print_producer("produce for buffer[%d]",idx);
    pthread_mutex_unlock(buffer_mutex[idx]);
}

char* get_string(){
    char* string;
    pthread_mutex_lock(products_num_mutex);
    while(is_empty()){
        print_consumer("is waiting for not empty buffer");
        pthread_cond_wait(full_buffer_cond,products_num_mutex);
    }
    pthread_mutex_lock(consumption_index_mutex);
    products_num--;
    pthread_mutex_unlock(products_num_mutex);

    pthread_mutex_lock(buffer_mutex[consumption_index]);
    print_consumer("is in buffer[%d]",consumption_index);
    string=buffer[consumption_index];
    buffer[consumption_index]=NULL;
    consumption_index++;
    print_consumer("consume buffer[%d]",consumption_index);
    if(consumption_index>=N){
        consumption_index=0;
    }   
    pthread_mutex_unlock(buffer_mutex[consumption_index]);
    pthread_mutex_unlock(consumption_index_mutex);

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
        pthread_mutex_destroy(products_num_mutex)!=0){
        perror("pthread_mutex_destroy");
        exit(EXIT_FAILURE);
    }    
    if(pthread_cond_destroy(empty_buffer_cond)!=0 ||
        pthread_cond_destroy(full_buffer_cond)!=0 ){
        perror("pthread_cond_destroy");
        exit(EXIT_FAILURE);  
    }  
     
}

void sig_hanlder(int signo) {
    for (int p = 0; p < P; p++)
        pthread_cancel(producer_threads[p]);
    for (int k = 0; k < K; k++)
        pthread_cancel(consumer_threads[k]);

    end();
    exit(EXIT_SUCCESS);
}

void init(){

    signal(SIGINT, sig_hanlder);
    if (nk > 0) signal(SIGALRM, sig_hanlder);

    buffer_mutex = malloc(N * sizeof(pthread_mutex_t*));
    for(int i=0;i<N;i++){
        buffer_mutex[i]=malloc(sizeof(pthread_mutex_t));
        if(pthread_mutex_init(buffer_mutex[i], NULL)!=0){   
            perror("pthread_mutex_init");
            exit(EXIT_FAILURE);  
        }
    }
    
    consumption_index_mutex=malloc(sizeof(pthread_mutex_t));
    products_num_mutex=malloc(sizeof(pthread_mutex_t));
    if(pthread_mutex_init(consumption_index_mutex, NULL)!=0 ||
        pthread_mutex_init(products_num_mutex, NULL)!=0){   
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
    
}

void *producer(void *args) {
    print_producer("started production");
    FILE *fp = fopen(file_path, "r");

    if(fp==NULL){
        perror(file_path);
        exit(EXIT_FAILURE);
    }
    
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, fp)) != -1) {
        print_producer("read next line");
        put_string(line);
    }

    if(fclose(fp)!=0){
        perror(file_path);
        exit(EXIT_FAILURE);
    }
    
    if (line)
        free(line);
    print_producer("end production");
    return NULL;
}
void config(char *config_path) {
    FILE *fp = fopen(config_path, "r");
    if(fp==NULL){
        perror(config_path);
        exit(EXIT_FAILURE);
    }
    fscanf(fp,"%d %d %d %s %d %d %d %d",&P,&K,&N,file_path,&L,&search_type,&print_type,&nk);


    if(fclose(fp)!=0){
        perror(config_path);
        exit(EXIT_FAILURE);
    }
}


void *consumer(void *args) {
    print_producer("started consumption");
    while(1){
        char*string = get_string();
        print_producer("consume %s",string);
        free(string);
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
        pthread_create(&producer_threads[p], NULL, producer, NULL);
    consumer_threads=malloc(K*sizeof(pthread_t));
    for (int k = 0; k < K; ++k)
        pthread_create(&consumer_threads[k], NULL, consumer, NULL);
    if (nk > 0) alarm(nk);
}

void join_threads(){
    for (int p = 0; p < P; ++p)
        pthread_join(producer_threads[p], NULL);
    //finished = 1;
    //pthread_cond_broadcast(&r_cond);
    for (int k = 0; k < K; ++k)
        pthread_join(consumer_threads[k], NULL);
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


