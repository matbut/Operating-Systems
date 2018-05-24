#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/types.h>
#include <errno.h>
#include "hairdresser.h"
#include <signal.h>

const char *hairdresser_path = "/hairdresser";
const char *hairdresser_sem_path = "/hairdresser_sem";
const char *client_ready_sem_path = "/client_ready_sem";
const char *barber_ready_sem_path = "/barber_ready_sem";
const char *awake_sem_path = "/awake_sem";

sem_t *hairdresser_sem;
sem_t *client_ready_sem;
sem_t *barber_ready_sem;
sem_t *awake_sem;

int shmfd;
Hairdresser* hairdresser;


#define PRINT(format, ...)  \
    printf("%ld us Barber ",get_time()); \
    printf(format, ##__VA_ARGS__); \
    printf("\n"); \
    fflush(stdout);

//BABER SLEEPING

void fall_asleep(){
    PRINT("is falling asleep");
    sem_give(awake_sem);
    //sem_take(awake_sem);
    sem_wait_taken(awake_sem);
    PRINT("is waking up");
}



void barber_hairdresser_init(int chairs_num){
    if((shmfd = shm_open(hairdresser_path,O_CREAT | O_RDWR,PERMISSIONS))==-1) {
        perror ("Barber: shm_open error");
        exit (EXIT_FAILURE);
    } 
    if(ftruncate(shmfd, (off_t) sizeof(Hairdresser))==-1) {
        perror ("Barber: ftruncate error");
        exit (EXIT_FAILURE);
    }  

    if((hairdresser = (Hairdresser*) mmap(NULL,sizeof(Hairdresser),PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0)) == -1){
        perror ("Barber: shmat error");
        exit (EXIT_FAILURE);
    } 
    if(
        (hairdresser_sem=sem_open(hairdresser_sem_path,O_CREAT,PERMISSIONS,1))==SEM_FAILED ||
        (client_ready_sem=sem_open(client_ready_sem_path,O_CREAT,PERMISSIONS,0))==SEM_FAILED ||
        (barber_ready_sem=sem_open(barber_ready_sem_path,O_CREAT,PERMISSIONS,0))==SEM_FAILED ||
        (awake_sem=sem_open(awake_sem_path,O_CREAT,PERMISSIONS,0))==SEM_FAILED
    ){
        perror ("Barber: sem_open error");
        exit (EXIT_FAILURE);
    }           

    hairdresser_init(hairdresser,chairs_num);
}

void barber_hairdresser_delete(){
    if(munmap((void *)hairdresser,sizeof(Hairdresser)) == -1){
        perror ("Barber: munmap error");
        exit (EXIT_FAILURE);
    } 

    if(close(shmfd) == -1){
        perror ("Barber: munmap error");
        exit (EXIT_FAILURE);
    } 

    if(
        sem_close(hairdresser_sem)==-1 ||
        sem_close(client_ready_sem)==-1 ||
        sem_close(barber_ready_sem)==-1 ||
        sem_close(awake_sem)==-1
    ){
        perror ("Barber: sem_close error");
        exit (EXIT_FAILURE);
    }   

    if(
        sem_unlink(hairdresser_sem_path)==-1 ||
        sem_unlink(client_ready_sem_path)==-1 ||
        sem_unlink(barber_ready_sem_path)==-1 ||
        sem_unlink(awake_sem_path)==-1 ||
        sem_unlink(hairdresser_path)==-1
        ){
        perror ("Barber: sem_unlink error");
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
        sem_take(hairdresser_sem);
        while(!queue_is_empty(hairdresser)){
            //Golibroda zaprasza do strzyżenia klienta, 
            pid_t pid=queue_get(hairdresser);
            PRINT("is inviting %d",pid);
            kill(pid,SIGUSR1);
            sem_give(hairdresser_sem);

            sem_take(client_ready_sem); // czekamy az sobie wygodnie usiądzie
            PRINT("starts cutting %d",hairdresser->barber_chair);
            PRINT("end cutting %d",hairdresser->barber_chair);
            sem_give(barber_ready_sem);
            sem_take(client_ready_sem); //czekamy aż klient pójdzie

            
            sem_take(hairdresser_sem);
            //Golibroda ponownie sprawdza poczekalnię.
        }
        sem_give(hairdresser_sem);
        //Jeśli w poczekalni nie ma klientów, golibroda zasypia. 
        fall_asleep();

        sem_take(client_ready_sem); // czekamy az sobie wygodnie usiądzie
        PRINT("starts cutting %d",hairdresser->barber_chair);
        PRINT("end cutting %d",hairdresser->barber_chair);
        sem_give(barber_ready_sem);
        sem_take(client_ready_sem); //czekamy aż klient pójdzie

    }


    /*

    int shmFd;
    char *shmArray;
    char *stringPtr;

    shmFd = shm_open(name, O_CREAT | O_RDWR, PERMISSIONS);
    ftruncate(shmFd, SIZE);

    shmArray = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);

    stringPtr = shmArray;
    sprintf(stringPtr, "%s %d \n", message,getpid());

    munmap(shmArray, SIZE);

    close(shmFd);
    */

    return 0;
}

