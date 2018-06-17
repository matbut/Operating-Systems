#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mqueue.h>
#include <unistd.h>
#include <sys/mman.h>

#define SHM_NAME "/kol_shm"
#define MAX_SIZE 1024

int main(int argc, char **argv)
{

    sleep(1);
    int val =0;
    /*******************************************
    Utworz/otworz posixowy obszar pamieci wspolnej "reprezentowany" przez SHM_NAME
    odczytaj zapisana tam wartosc i przypisz ja do zmiennej val
    posprzataj
    *********************************************/
    int shmid;
    if((shmid=shm_open(SHM_NAME,O_RDONLY,0666))==-1){
        printf("shm_open error");
        return 1;
    }


    int*v=(int*)mmap(NULL,sizeof(int),PROT_READ,MAP_SHARED,shmid,0);

    

	printf("%d square is: %d \n",val, val*val);

    if(close(shmid)==-1){
        printf("close error");
        return 1;
    }

    return 0;
}
