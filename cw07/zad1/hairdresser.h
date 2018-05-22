#ifndef HAIRDRESSER_H
#define HAIRDRESSER_H

#define MAX_CHAIRS_NUM 1024

#define PROJECT_ID 'M'

#define PERMISSIONS 0666

#include <sys/types.h>

//const char *name = "/sysOpy";

#define HAIRDRESSER_SEM 0
#define CLIENT_READY_SEM 1
#define BARBER_READY_SEM 2
#define AWAKE_SEM 3

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

void sem_wait_taken(int num);
void sem_take(int num);
void sem_give(int num);
int sem_is_taken(int num);
int semval(int num);

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