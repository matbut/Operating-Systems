#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_ARGS_NUM 10
#define MAX_PIPES_NUM 10
int pipefd[2][2];

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

    char *commands[MAX_ARGS_NUM+1][MAX_PIPES_NUM+1];
    char *lines[MAX_ARGS_NUM+1];

    for(int i=0;i<MAX_ARGS_NUM+1;i++){
        lines[i]=NULL;
        for(int j=0;j<MAX_PIPES_NUM+1;j++)
            commands[i][j]=NULL;
    }

    int args_num;
    int commands_num;
    char *word;

    while(getline(&line, &length, file) != -1){
        args_num = 0;
        
        word= strtok (line,"|\n\t");
        while (word != NULL && args_num<MAX_ARGS_NUM){

            lines[args_num]=word;
            args_num++;
            word = strtok (NULL, "|\n\t");
        }
        lines[args_num]=NULL;
        if(args_num>=MAX_ARGS_NUM){
            perror("to many arguments");
            exit(EXIT_FAILURE);
        }
        for(int i=0;i<args_num;i++){
            commands_num=0;
            word = strtok(lines[i]," ");
            while(word != NULL && commands_num<MAX_PIPES_NUM){
                commands[i][commands_num]=word;
                commands_num++;
                word = strtok (NULL, " ");
            } 
            commands[i][commands_num]=NULL;

            if(commands_num>MAX_PIPES_NUM){
                perror("to many pipes");
                exit(EXIT_FAILURE);
            }  
        }
  
        int i;
        for(i=0;i<args_num;i++){
            //printf("  executing %s\n",commands[i][0]);
            if (pipe(pipefd[i%2]) == -1) {
                perror("pipe error");
                exit(EXIT_FAILURE);
            }

            pid_t pid=fork();

            if(pid<0){
                perror("new process failed");
                exit(EXIT_FAILURE);
            }else if (pid==0){
                if (i > 0){
                    close(pipefd[(i+1)%2][1]);
                    if (dup2(pipefd[(i+1)%2][0], STDIN_FILENO) < 0){
                        perror("in dup2 error");
                        exit(EXIT_FAILURE);
                    }
                }if (i < commands_num - 1) {
                    close(pipefd[i%2][0]);
                    if (dup2(pipefd[i%2][1], STDOUT_FILENO) < 0) {
                        perror("out dup2 error");
                        exit(EXIT_FAILURE);
                    }
                }if(execvp(commands[i][0],commands[i])==-1){
                    printf ("process %s failed: %s\n",commands[i][0], strerror(errno));
                    exit(EXIT_FAILURE);
                }
            }else{
                if (i < commands_num - 1)
                    close(pipefd[i%2][1]);
                if (i > 0)
                    close(pipefd[(i+1)%2][0]);
            }
        }

        while (wait(NULL)) 
            if (errno == ECHILD) 
                break;
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
