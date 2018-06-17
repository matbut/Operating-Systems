#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void sighandler(int signum, siginfo_t*siginfo, void *ucontext){
	printf("%d\n",siginfo->si_value.sival_int);
}

int main(int argc, char* argv[]) {



    if(argc != 3){
        printf("Not a suitable number of program parameters\n");
        return 1;
    }

    //..........


    int child = fork();
    if(child == 0) {
	sigset_t set;

	if(sigfillset(&set)!=0){
		perror("sigfillset error");
		exit(-1);
	}

	if(sigdelset(&set,SIGUSR1)!=0){
		perror("sigfillset error");
		exit(-1);
	}
	
	if(sigprocmask(SIG_SETMASK,&set,NULL)!=0){
		perror("sigprocmask error");
		exit(-1);
	}

	struct sigaction action;
    	action.sa_sigaction = &sighandler;
    	action.sa_mask=set;
	action.sa_flags =  SA_RESTART | SA_SIGINFO;
	
	if(sigaction(SIGUSR1,&action,NULL)){
		perror("sigaction error");
		exit(-1);
	}

	//SA_SIGINFO  flag to sigaction(2), then it can obtain this data via
       //the si_value field of the siginfo_t 

        //zablokuj wszystkie sygnaly za wyjatkiem SIGUSR1
        //zdefiniuj obsluge SIGUSR1 w taki sposob zeby proces potomny wydrukowal
        //na konsole przekazana przez rodzica wraz z sygnalem SIGUSR1 wartosc

	pause();
    }
    else {
	sleep(1);

	union sigval val;


	val.sival_int = atoi(argv[1]);

	if(sigqueue(child,atoi(argv[2]),val)!=0){
		perror("sigqueue error");
		exit(-1);
	}

        //wyslij do procesu potomnego sygnal przekazany jako argv[2]
        //wraz z wartoscia przekazana jako argv[1]

        //int status;
        //waitpid(child, &status, 0);
	sleep(1);

    }

    return 0;
}
