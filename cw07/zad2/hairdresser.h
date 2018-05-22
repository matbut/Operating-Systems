#ifndef HAIRDRESSER_H
#define HAIRDRESSER_H

#define MAX_CHAIRS_NUM 1024

#define PROJECT_ID 'M'

#include <sys/types.h>
#include <semaphore.h>

#define PERMISSIONS 0666


int semid;

typedef struct Queue{
    int chairs_num;
    int occupied_num;
    pid_t chairs[MAX_CHAIRS_NUM];
    int first_chair;
} Queue;

typedef struct Hairdresser{
    Queue queue;
    pid_t barber_chair;
} Hairdresser;

//BASE QUEUE

void sem_wait_taken(sem_t *sem);
void sem_take(sem_t *sem);
void sem_give(sem_t *sem);
int sem_is_taken(sem_t *sem);

//QUEUE

int queue_is_empty(Hairdresser * hairdresser);
int queue_is_full(Hairdresser * hairdresser);
int queue_put(Hairdresser * hairdresser,pid_t client);
pid_t queue_get(Hairdresser * hairdresser);

//HAIRSRESSER

void hairdresser_init(Hairdresser * hairdresser,int chairs_num);
//GETTIME

long get_time();


#endif