#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <limits.h>

int main(int argc, char **argv) {
    if(argc != 2) {
        printf("Ilegal arguments number");
        exit(EXIT_FAILURE);
    }

    if(mkfifo(argv[1], S_IRUSR	| S_IWUSR) != 0){
        perror("Fifo make error");
        exit(EXIT_FAILURE);
    }

    FILE *pipe = fopen(argv[1], "r");
    if (pipe == NULL) {
        perror("Pipe open error");
        exit(EXIT_FAILURE);
    }
    char buff[PIPE_BUF];
    while(fgets(buff, PIPE_BUF, pipe) != NULL){
        printf("Master received: %s", buff);
    }
    fclose(pipe);

    return 0;
}