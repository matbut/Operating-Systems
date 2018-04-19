#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <limits.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    srand(getpid());

    if(argc != 3) {
        printf("Ilegal arguments number");
        exit(EXIT_FAILURE);
    }

    FILE *date;

    int messages_num = (int) strtol(argv[2], '\0', 10);

    int fifo = open(argv[1],O_WRONLY);
    if (fifo < 0){
        perror("Fifo open error");
        exit(EXIT_FAILURE);
    } 

    printf("Slave %d will be sending %d messages\n", getpid(),messages_num);

    char date_message[PIPE_BUF];
    char full_message[PIPE_BUF];
    for (int i = 0; i < messages_num; i++) {
        date = popen("date", "r");
        fgets(date_message, PIPE_BUF, date);
        sprintf(full_message, "Slave %d is sending %s", getpid(),date_message);
        write(fifo, full_message, strlen(full_message));
        fclose(date);
        sleep((rand() % 4 + 2));
    }
    close(fifo);

    printf("Slave %d ends sending\n", getpid());

    return 0;
}
