#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main ( int argc, char *argv[] )
{
    int i, pid;

    if(argc != 2){
        printf("Not a suitable number of program arguments");
        exit(2);
    }else {
       for (i = 0; i < atoi(argv[1]); i++) {
		int child = fork();
		if(child == 0){
			printf("I am %d child, my pid is %d, my parent pid is %d\n",i,getpid(),getppid);
			exit(0);
		}
        //*********************************************************
        //Uzupelnij petle w taki sposob aby stworzyc dokladnie argv[1] procesow potomnych, bedacych dziecmi
        //   tego samego procesu macierzystego.
           // Kazdy proces potomny powinien:
               // - "powiedziec ktorym jest dzieckiem",
                //-  jaki ma pid,
                //- kto jest jego rodzicem
           //******************************************************
		
        }
	while (wait(NULL) > 0);
	//while(waitpid(-1, NULL, WNOHANG));
    }
    return 0;
}
