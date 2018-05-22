# define _GNU_SOURCE
#include "hairdresser.h"
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/types.h>
#include <time.h>
#include <stdio.h>

//BASE QUEUE

void init(Queue *q,int chairs_num){
    if(chairs_num>MAX_CHAIRS_NUM){
        printf("too many chairs");
        exit (EXIT_FAILURE);
    }
    q->chairs_num=chairs_num;
    q->occupied_num=0;
    q->first_chair=0;
}

int is_empty(Queue *q){
    return q->occupied_num==0;
}

int is_full(Queue *q){
    return q->occupied_num==q->chairs_num;
}

int put(Queue *q,pid_t client){
    if(is_full(q))
        return -1;

    ++q->occupied_num;
    q->chairs[(q->first_chair+q->occupied_num-1)%q->chairs_num]=client;
    return 0;
}

pid_t get(Queue *q){
    if(is_empty(q))
        return -1; 
    q->occupied_num--;
    return q->chairs[(q->first_chair+q->occupied_num)%q->chairs_num];    
}

int queue_is_empty(Hairdresser * hairdresser){
    return is_empty(&hairdresser->queue);
}

int queue_is_full(Hairdresser * hairdresser){
    return is_full(&hairdresser->queue);
}

int queue_put(Hairdresser * hairdresser,pid_t client){
    return put(&hairdresser->queue,client);
}

pid_t queue_get(Hairdresser * hairdresser){
   return get(&hairdresser->queue);
}


//SEM

void sem(int num,int op){
    struct sembuf sops;
    sops.sem_flg=0;
    sops.sem_num=num;
    sops.sem_op=op;
    if(semop(semid, &sops, (size_t) 1)==-1){
        perror ("semop error");
        exit (EXIT_FAILURE);
    }
}

void sem_wait_taken(int num){
    sem(num,0);
}

void sem_take(int num){
    sem(num,-1);
}

void sem_give(int num){
    sem(num,1);
}

int sem_is_taken(int num){
    int semval;
    if ((semval = semctl(semid, num, GETVAL, 0)) == -1){
        perror ("semctl GETVAL error");
        exit (EXIT_FAILURE);
    } 
    return semval==0;
}

//HAIRSRESSER

void hairdresser_init(Hairdresser * hairdresser,int chairs_num){
    init(&hairdresser->queue,chairs_num);
    hairdresser->barber_chair=-1;
}

//GETTIME

long get_time() {
    struct timespec marker;
    if (clock_gettime(CLOCK_MONOTONIC, &marker) == -1){
        perror ("clock_gettime error");
        exit (EXIT_FAILURE);   
    }
    return marker.tv_nsec / 1000;
}

