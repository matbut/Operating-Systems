#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mqueue.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>

#define SHM_NAME "/kol_shm"
#define MAX_SIZE 1024

int main(int argc, char **argv)
{


    
   if(argc !=2){
    printf("Not a suitable number of program parameters\n");
    return(1);
   }

   /***************************************
   Utworz/otworz posixowy obszar pamieci wspolnej "reprezentowany" przez SHM_NAME
   zapisz tam wartosc przekazana jako parametr wywolania programu
   posprzataj
   *****************************************/

    int shmid;
    if((shmid=shm_open(SHM_NAME,O_WRONLY | O_CREAT,0666))==-1){
        printf("shm_open error");
        return 1;
    }

    if(ftruncate(shmid,sizeof(int))==-1){
        printf("ftruncate error");
        return 1;
    }

    int*val=(int*) mmap(NULL,sizeof(int),PROT_WRITE,MAP_SHARED,shmid,0);
    

    
    
    if(close(shmid)==-1){
        printf("close error");
        return 1;
    }
    if(unlink(SHM_NAME)==-1){
        printf("unlink error");
        perror("");
        return 1;
    } 
    return 0;
}
