#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <errno.h>
#include "hairdresser.h"
#include <signal.h>

int shmid;
Hairdresser* hairdresser;

#define PRINT(format, ...)  \
    printf("%ld us Barber ",get_time()); \
    printf(format, ##__VA_ARGS__); \
    printf("\n"); \
    fflush(stdout);

//BABER SLEEPING

void fall_asleep(){
    PRINT("is falling asleep");
    sem_give(AWAKE_SEM);
    //sem_take(AWAKE_SEM);
    sem_wait_taken(AWAKE_SEM);
    PRINT("is waking up");
}

//

void barber_hairdresser_init(int chairs_num){
    char* path = getenv("HOME");
    if(path == NULL){
        perror ("Barber: server getenv \"HOME\" error");
        exit (EXIT_FAILURE);  
    }

    key_t key;
    if ((key = ftok (path, PROJECT_ID))==-1) {
        perror ("Barber: ftok error");
        exit (EXIT_FAILURE);
    }

    if((shmid = shmget(key, sizeof(Hairdresser),IPC_CREAT | PERMISSIONS ))==-1) {
        perror ("Barber: shmget error");
        exit (EXIT_FAILURE);
    } 

    if((hairdresser = (Hairdresser*) shmat(shmid, NULL,0)) == -1){
        perror ("Barber: shmat error");
        exit (EXIT_FAILURE);
    } 

    //SEM

    if((semid=semget(key,4, IPC_CREAT | PERMISSIONS ))==-1){
        perror ("semget error");
        exit (EXIT_FAILURE);
    } 

    if (
        (semctl(semid, HAIRDRESSER_SEM, SETVAL, 1) == -1) ||
        (semctl(semid, CLIENT_READY_SEM, SETVAL, 0) == -1) ||
        (semctl(semid, BARBER_READY_SEM, SETVAL, 0) == -1) ||
        (semctl(semid, AWAKE_SEM, SETVAL, 0) == -1)
    ){
        perror ("semctl error");
        exit (EXIT_FAILURE);
    } 

    hairdresser_init(hairdresser,chairs_num);
}

void barber_hairdresser_delete(){
    if(shmdt((void*) hairdresser) == -1){
        perror ("Barber: shmdt error");
        exit (EXIT_FAILURE);
    } 

    if((shmctl(shmid, IPC_RMID, NULL))==-1){
        perror ("Barber: shmctl error");
        exit (EXIT_FAILURE);
    } 
    if((semctl(semid,0, IPC_RMID, NULL))==-1){
        perror ("Barber: semctl error");
        exit (EXIT_FAILURE);
    } 
}
void at_exit(){
    barber_hairdresser_delete();
    _exit(EXIT_SUCCESS);
}

void sigterm_handler(int signo){
    exit(signo);
}

int main(int argc, char **argv)
{
    if(argc!=2){
        printf("Barber: Ilegal arguments number");
        exit(EXIT_FAILURE);
    }

    PRINT("is born");

    if (atexit(at_exit)== -1){
        perror("Barber: atexit error");
        exit(EXIT_FAILURE);
    }
    if (signal(SIGTERM, sigterm_handler) == SIG_ERR){
        perror("Barber: signal error");
        exit(EXIT_FAILURE);
    }
    
    if (signal(SIGINT, sigterm_handler) == SIG_ERR){
        perror("Barber: signal error");
        exit(EXIT_FAILURE);
    }
    
    barber_hairdresser_init(atoi(argv[1]));

    while(1){
        //Golibroda sprawdza, czy w poczekalni oczekują klienci.
        sem_take(HAIRDRESSER_SEM);
        while(!queue_is_empty(hairdresser)){
            //Golibroda zaprasza do strzyżenia klienta, 
            pid_t pid=queue_get(hairdresser);
            PRINT("is inviting %d",pid);
            kill(pid,SIGUSR1);
            sem_give(HAIRDRESSER_SEM);

            sem_take(CLIENT_READY_SEM); // czekamy az sobie wygodnie usiądzie
            PRINT("starts cutting %d",hairdresser->barber_chair);
            PRINT("end cutting %d",hairdresser->barber_chair);
            sem_give(BARBER_READY_SEM);
            sem_take(CLIENT_READY_SEM); //czekamy aż klient pójdzie

            
            sem_take(HAIRDRESSER_SEM);
            //Golibroda ponownie sprawdza poczekalnię.
        }
        sem_give(HAIRDRESSER_SEM);
        //Jeśli w poczekalni nie ma klientów, golibroda zasypia. 
        fall_asleep();

        sem_take(CLIENT_READY_SEM); // czekamy az sobie wygodnie usiądzie
        PRINT("starts cutting %d",hairdresser->barber_chair);
        PRINT("end cutting %d",hairdresser->barber_chair);
        sem_give(BARBER_READY_SEM);
        sem_take(CLIENT_READY_SEM); //czekamy aż klient pójdzie

    }

    return 0;
}

