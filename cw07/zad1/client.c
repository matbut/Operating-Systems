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
#include "hairdresser.h"

pid_t pid;
Hairdresser* hairdresser;
int ready=0;

#define PRINT(format, ...)  \
    printf("%ld us Client %d ",get_time(),pid); \
    printf(format, ##__VA_ARGS__); \
    printf("\n"); \
    fflush(stdout);

//BABER SLEEPING

void wake_barber(){
    PRINT("is waking up barber");
    sem_take(AWAKE_SEM);
}

int is_sleeping(){
    return !sem_is_taken(AWAKE_SEM);
}

//

void client_hairdresser_init(){
    char* path = getenv("HOME");
    if(path == NULL){
        perror ("Client: server getenv \"HOME\" error");
        exit (EXIT_FAILURE);  
    }

    key_t key;
    if ((key = ftok (path, PROJECT_ID))==-1) {
        perror ("Client: ftok error");
        exit (EXIT_FAILURE);
    }
    int shmid;
    if((shmid = shmget(key, sizeof(Hairdresser),0))==-1) {
        perror ("Client: shmget error");
        exit (EXIT_FAILURE);
    } 

    if((hairdresser = (Hairdresser*) shmat(shmid, NULL,0)) == -1){
        perror ("Client: shmat error");
        exit (EXIT_FAILURE);
    } 

    if((semid=semget(key,4,0))==-1){
        perror ("Client: semget error");
        exit (EXIT_FAILURE);
    } 
}

void at_exit(){
    if(shmdt((void*) hairdresser) == -1){
        perror ("Client: shmdt error");
        exit (EXIT_FAILURE);
    } 
    _exit(EXIT_SUCCESS);
}



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
        sem_take(HAIRDRESSER_SEM);
        PRINT("is coming");
        //nowy klient sprawdza,
        //czy w poczekalni jest wolne krzesło. 
        while(queue_is_full(hairdresser)){
            //W przeciwnym razie klient opuszcza zakład.
            PRINT("is leaving because of full queue");
            sem_give(HAIRDRESSER_SEM);
            sem_take(HAIRDRESSER_SEM);
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
            sem_give(HAIRDRESSER_SEM);
            while(!ready);
            ready=0;
            sem_take(HAIRDRESSER_SEM);
        }
        //siada na krześle w gabinecie i jest strzyżony.
        PRINT("is sitting");
        hairdresser->barber_chair=pid; 
        sem_give(HAIRDRESSER_SEM);
        sem_give(CLIENT_READY_SEM);

        sem_take(BARBER_READY_SEM); // czekamy az golibroda obetnie
        PRINT("was cut and is leaving"); //Gdy golibroda skończy strzyżenie, klient opuszcza zakład. 
        sem_give(CLIENT_READY_SEM);
    }
    
    PRINT("is dead");

    return 0;
}
