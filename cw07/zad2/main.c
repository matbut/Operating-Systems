#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>


int main(int argc, char **argv)
{
    if(argc!=3){
        printf("Main: Ilegal arguments number");
        exit(EXIT_FAILURE);
    }

    for(int i=0;i<atoi(argv[1]);i++){
        int child_pid=fork();
        if(child_pid==0){
            execl("client","client",argv[2]);
        }else if(child_pid<0){
            perror("Fork error");
            exit(EXIT_FAILURE);
        }
    }
    int wpid,status;
    while ((wpid = wait(&status)) > 0);

}