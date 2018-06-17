#include <fcntl.h>           
#include <sys/stat.h>        
#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#define KEY  "/queue"

int main() {
    sleep(1);
    int val = 0;


	/**********************************
	Otworz kolejke systemu Posix "reprezentowana" przez KEY
	**********************************/


    mqd_t mqdes;
    if((mqdes=mq_open(KEY,O_RDWR))==-1){
        printf("mq_open error");
        return 1;
    }

	
 	
    /**********************************
    Odczytaj zapisane w kolejce wartosci i przypisz je do zmiennej val
    obowiazuja funkcje systemu Posix
    ************************************/

    char *msg=malloc(10*sizeof(char));

    if(mq_receive(mqdes,msg, 10, NULL) == -1) {
        printf("mq_receive error");
        perror("");
        return 1;
    }
    val=atoi(msg);
    printf("message received: %d\n",val);
        
    

	/*******************************
	posprzataj
	********************************/
    if(mq_close(mqdes)==-1){
        printf("mq_close error");
        return 1;
    }
    if(mq_unlink(KEY)==-1){
        printf("mq_unlink error");
        return 1;
    }

    return 0;
    }
