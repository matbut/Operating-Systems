#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define KEY  "./queuekey"

struct msgbuf {
    long mtype;
    char mtext[80];
};

int main() {
    sleep(1);
    int val = 0;


	/**********************************
	Otworz kolejke systemu V "reprezentowana" przez KEY
	**********************************/
    key_t key=ftok(KEY,'A');
    int msqid;
    if((msqid=msgget(key,0))==-1){
        printf("msgget error");
        return 1;
    }

	
 	
    /**********************************
    Odczytaj zapisane w kolejce wartosci i przypisz je do zmiennej val
    obowiazuja funkcje systemu V
    ************************************/

    struct msgbuf msg;

    if (msgrcv(msqid, (void *) &msg, sizeof(msg.mtext),0,0) == -1) {
        printf("msgrcv error");
        return 1;
    }
    val=atoi(msg.mtext);
    printf("message received: %d\n",val);
        
    

	/*******************************
	posprzataj
	********************************/
    if(msgctl(msqid,IPC_RMID,NULL)==-1){
        printf("msgctl error");
        return 1;
    }

    return 0;
    }
