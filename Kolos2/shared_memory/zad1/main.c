#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

#define MAX 20
#define KEY "./dupa"

int  main(){
	int shmid, i;
	int *buf;
	//Wygeneruj klucz dla kolejki na podstawie KEYi znaku 'a'

	int key = ftok(KEY, 'D');



	//Utwórz segment pamięci współdzielonej shmid o prawach dostępu 600, rozmiarze MAX 
	//jeśli segment już istnieje, zwróć błąd, jeśli utworzenie pamięci się nie powiedzie zwróć błąd i wartość 1
	if((shmid=shmget(key,MAX, IPC_CREAT | IPC_EXCL | 0666))==-1){
		printf("shmget error");
		perror("");
		return 1;
	}

	//Przyłącz segment pamięci współdzielonej do procesu do buf, obsłuż błędy i zwróć 1

	if((buf=shmat(shmid,NULL,0))==-1){
		printf("shmat error");
		return 1;
	}

	for (i=0; i<MAX; i++){
		buf[i]=i*i;
		printf("Wartość: %d \n",buf[i]);
	}
	printf ("Memory written\n");
	//odłącz i usuń segment pamięci współdzielonej

	if(shmdt(buf)==-1){
		printf("shmdt error");
		return 1;
	}
	if(shmctl(shmid,IPC_RMID,NULL)==-1){
		printf("shmctl error");
		return 1;
	}

	return 0;
}
