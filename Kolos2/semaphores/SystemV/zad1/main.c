#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>
#include <sys/wait.h>

#define FILE_NAME "common.txt"
#define SEM_NAME "/kol_sem"


int main(int argc, char** args){

   if(argc !=4){
    printf("Not a suitable number of program parameters\n");
    return(1);
   }

    /**************************************************
    Stworz semafor systemu V
    Ustaw jego wartosc na 1
    ***************************************************/

    key_t key=ftok(SEM_NAME,'A');

    int semid;
    
    if((semid=semget(key,1,IPC_CREAT | 0666))==-1){
      printf("semget error");
      return 1;
    }


    if(semctl(semid,0,SETVAL,1)==-1){
      printf("semctl SETVAL error");
      return 1;
    } 

    struct sembuf sops;
    sops.sem_num=0;
    sops.sem_flg=0;
    
     
     int fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC , 0644);
     
     int parentLoopCounter = atoi(args[1]);
     int childLoopCounter = atoi(args[2]);
        
     char buf[20];
     pid_t childPid;
     int max_sleep_time = atoi(args[3]);
     


     if(childPid=fork()){
      int status = 0;
      srand((unsigned)time(0)); 

        while(parentLoopCounter--){
    	    int s = rand()%max_sleep_time+1;
    	    sleep(s);    
            
      /*********************************
	    Początek sekcji krytycznej
	    **********************************/
          sops.sem_op=-1;
          if (semop(semid,&sops,1)==-1){
            printf("semop error");
            return 1;
          }
        
          sprintf(buf, "Wpis rodzica. Petla %d. Spalem %d\n", parentLoopCounter,s);
	        write(fd, buf, strlen(buf));
	        write(1, buf, strlen(buf));
            
	    /*********************************
	    Koniec sekcji krytycznej
	    **********************************/
          sops.sem_op=1;
          if (semop(semid,&sops,1)==-1){
            printf("semop error");
            return 1;
          }

        }
        waitpid(childPid,&status,0);
     }
     else{

	srand((unsigned)time(0)); 
        while(childLoopCounter--){

	    int s = rand()%max_sleep_time+1;
            sleep(s);                
            

	    /*****************************************
	    sekcja krytyczna zabezpiecz dostep semaforem
	    **********************************************/
          sops.sem_op=-1;
          if (semop(semid,&sops,1)==-1){
            printf("semop error");
            return 1;
          }
            
            sprintf(buf, "Wpis dziecka. Petla %d. Spalem %d\n", childLoopCounter,s);
            write(fd, buf, strlen(buf));
	    write(1, buf, strlen(buf));

	    /*********************************
	    Koniec sekcji krytycznej
	    **********************************/
          sops.sem_op=1;
          if (semop(semid,&sops,1)==-1){
            printf("semop error");
            return 1;
          }
        }
        _exit(0);
     }
     
    /*****************************
    posprzataj semafor
    ******************************/

    
    if(semctl(semid,0,IPC_RMID)==-1){
      printf("semctl IPC_RMID error");
      return 1;
    } 

    close(fd);
    return 0;
}
     
        
