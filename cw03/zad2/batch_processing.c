#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_ARGS_NUM 10

void printHelp(char *progName);

int main (int argc, char **argv){

    if (argc!=2) {
        printf("Ilegal arguments number");
        printHelp(argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE *file = fopen(argv[1], "r");
    if(file==NULL){
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }
        
    char* line;
    size_t length;

    char *words[MAX_ARGS_NUM+1];
    words[MAX_ARGS_NUM]=NULL;

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
            if(execvp(words[0], words)==-1){
                printf ("process %s failed: %s\n",words[0], strerror(errno));
                exit(EXIT_FAILURE);
            }
        }else{
            int status;
            if (wait(&status) < 0){
				perror(line);
				exit(EXIT_FAILURE);
            }
            if (status!=EXIT_SUCCESS) {
                exit(EXIT_FAILURE);
            }
        }
        
    }

    if(fclose(file)!=0){
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

void printHelp(char *progName){
  printf("Usage help\n");
  printf("%s <file> \n",progName);
}
