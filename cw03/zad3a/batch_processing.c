#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>

#define MAX_ARGS_NUM 10

void printHelp(char *);
void print_usage(char*,struct rusage);

struct rlimit cpu_time_limit;
struct rlimit memory_limit;

int main (int argc, char **argv){

    if (argc!=4) {
        printf("Ilegal arguments number");
        printHelp(argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE *file = fopen(argv[1], "r");
    if(file==NULL){
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }
            
    cpu_time_limit.rlim_cur = cpu_time_limit.rlim_max = (rlim_t) strtol(argv[2], NULL, 10);
    memory_limit.rlim_cur = memory_limit.rlim_max = (rlim_t) strtol(argv[3], NULL, 10)*1024*1024;
        
    char* line;
    size_t length;

    char *words[MAX_ARGS_NUM+1];
    words[MAX_ARGS_NUM]=NULL;

    struct rusage usage;

    while(getline(&line, &length, file) != -1){
        int args_num = 0;

        char * word = strtok (line," \n\t");
        while (word != NULL && args_num<MAX_ARGS_NUM){

            words[args_num]=word;
            args_num++;
            word = strtok (NULL, " \n\t");
        }
        words[args_num]=NULL;
        if(args_num>=MAX_ARGS_NUM){
            perror("to many arguments");
            exit(EXIT_FAILURE);
        }

        pid_t pid=fork();

        if(pid<0){
            perror("new process failed");
            exit(EXIT_FAILURE);
        }else if (pid==0){

    		if (setrlimit(RLIMIT_CPU, &cpu_time_limit) < 0){
    			perror("Can't set cpu time limit");
    			exit(EXIT_FAILURE);
    		}

    		if (setrlimit(RLIMIT_AS, &memory_limit) < 0){
    			perror("Can't set memory limit");
    			exit(EXIT_FAILURE);
    		}

            if(execvp(words[0], words)==-1){
                printf ("process %s failed: %s\n",words[0], strerror(errno));
                exit(EXIT_FAILURE);
            }
        }else{
            int status;
            if (wait3(&status,0,&usage) < 0){
				perror("");
				exit(EXIT_FAILURE);
            }
            print_usage(words[0],usage);
            if (WEXITSTATUS(status)){
                exit(EXIT_FAILURE);
            }
            if (WIFSIGNALED(status)){
				printf("process %s has been terminated, maybe reached one of its resource usage limit\n",words[0]);
            }
        }
    }

    if(fclose(file)!=0){
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}


void print_usage(char*command,struct rusage usage)
{
	printf("+----------------------------+\n");
    printf("| Command %18s |\n",command);
	printf("| System time:%5ld.%06ld s |\n", \
		usage.ru_stime.tv_sec,usage.ru_stime.tv_usec);
	printf("| User time:  %5ld.%06ld s |\n" , \
		usage.ru_utime.tv_sec,usage.ru_utime.tv_usec);
    printf("+----------------------------+\n");

}

void printHelp(char *progName){
  printf("Usage help\n");
  printf("%s <file> <cpu_time_limit [s]> <memory_limit [Mb]>\n",progName);
}
