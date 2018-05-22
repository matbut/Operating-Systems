# define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <semaphore.h>
#include "hairdresser.h"

const char *hairdresser_path = "/hairdresser";
const char *hairdresser_sem_path = "/hairdresser_sem";
const char *client_ready_sem_path = "/client_ready_sem";
const char *barber_ready_sem_path = "/barber_ready_sem";
const char *awake_sem_path = "/awake_sem";

int pid;

sem_t *hairdresser_sem;
sem_t *client_ready_sem;
sem_t *barber_ready_sem;
sem_t *awake_sem;

int shmfd;
Hairdresser* hairdresser;

#define PRINT(format, ...)  \
    printf("%ld us Client %d ",get_time(),pid); \
    printf(format, ##__VA_ARGS__); \
    printf("\n"); \
    fflush(stdout);

//BABER SLEEPING

void wake_barber(){
    PRINT("is waking up barber");
    sem_take(awake_sem);
}

int is_sleeping(){
    return !sem_is_taken(awake_sem);
}


//MAIN

void client_hairdresser_init(){
    if((shmfd = shm_open(hairdresser_path,O_RDWR,PERMISSIONS))==-1) {
        perror ("Client: shm_open error");
        exit (EXIT_FAILURE);
    } 

    if((hairdresser = (Hairdresser*) mmap(NULL,sizeof(Hairdresser),PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0)) == -1){
        perror ("Client: shmat error");
        exit (EXIT_FAILURE);
    } 

    if(
        (hairdresser_sem=sem_open(hairdresser_sem_path,0))==SEM_FAILED ||
        (client_ready_sem=sem_open(client_ready_sem_path,0))==SEM_FAILED ||
        (barber_ready_sem=sem_open(barber_ready_sem_path,0))==SEM_FAILED ||
        (awake_sem=sem_open(awake_sem_path,0))==SEM_FAILED
    ){
        perror ("Client: sem_open error");
        exit (EXIT_FAILURE);
    } 
}

void at_exit(){
    if(munmap((void *)hairdresser,sizeof(Hairdresser)) == -1){
        perror ("Client: munmap error");
        exit (EXIT_FAILURE);
    } 

    if(close(shmfd) == -1){
        perror ("Client: munmap error");
        exit (EXIT_FAILURE);
    } 

    if(
        sem_close(hairdresser_sem)==-1 ||
        sem_close(client_ready_sem)==-1 ||
        sem_close(barber_ready_sem)==-1 ||
        sem_close(awake_sem)==-1
    ){
        perror ("Client: sem_close error");
        exit (EXIT_FAILURE);
    }  
    _exit(EXIT_SUCCESS);
}

int ready=0;

void sigusr1_handler(int signo){
    ready=1;
}

int main(int argc, char **argv)
{

    if(argc!=2){
        printf("Client: Ilegal arguments number");
        exit(EXIT_FAILURE);
    }

    if (signal(SIGUSR1,sigusr1_handler) == SIG_ERR){
        perror("signal error");
        exit(EXIT_FAILURE);
    }    
    if (atexit(at_exit)== -1){
        perror("Barber: atexit error");
        exit(EXIT_FAILURE);
    }

    pid=getpid();

    PRINT("is born");

    client_hairdresser_init();

    for(int i=0;i<atoi(argv[1]);i++){
        sem_take(hairdresser_sem);
        PRINT("is coming");
        //nowy klient sprawdza,
        //czy w poczekalni jest wolne krzesło. 
        while(queue_is_full(hairdresser)){
            //W przeciwnym razie klient opuszcza zakład.
            PRINT("is leaving because of full queue");
            sem_give(hairdresser_sem);
            sem_take(hairdresser_sem);
            PRINT("is coming");
        }

        //Po przyjściu do zakładu klient sprawdza co robi golibroda
        //Jeśli golibroda śpi,
        if(is_sleeping()){
            //klient budzi go, 
            wake_barber();
        }else{                 
            //Jeśli golibroda strzyże innego klienta, 
            // to klient siada w poczekalni. 
            queue_put(hairdresser,pid);
            PRINT("is waiting in queue"); 
            sem_give(hairdresser_sem);
            while(!ready);
            ready=0;
            sem_take(hairdresser_sem);
        }
        //siada na krześle w gabinecie i jest strzyżony.
        PRINT("is sitting");
        hairdresser->barber_chair=pid; 
        sem_give(hairdresser_sem);
        sem_give(client_ready_sem);

        sem_take(barber_ready_sem); // czekamy az golibroda obetnie
        PRINT("was cut and is leaving"); //Gdy golibroda skończy strzyżenie, klient opuszcza zakład. 
        sem_give(client_ready_sem);
    }
    
    PRINT("is dead");

    return 0;
}
