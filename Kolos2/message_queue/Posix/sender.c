#include <fcntl.h>           
#include <sys/stat.h>        
#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define KEY "/queue"

int main(int argc, char* argv[]){

    if(argc !=2){
    printf("Not a suitable number of program parameters\n");
    return(1);
    }

    struct mq_attr new_queue_attrs;

    // new_queue_attrs.mq_flags = 0;
    new_queue_attrs.mq_maxmsg = 10;
    new_queue_attrs.mq_msgsize = 10;
    // new_queue_attrs.mq_curmsgs = -1;


    mqd_t mqdes;
    if((mqdes = mq_open(KEY,O_CREAT | O_RDWR,0666,&new_queue_attrs))==-1){
        printf("mq_open error");
        return 1;
    }

    char msg[10];
    strcpy(msg,argv[1]);

    if (mq_send(mqdes,msg, sizeof(msg),20) == -1) {
        printf("mq_send error");
        return 1;
    }
    printf("sent: %s\n", msg);

    if(mq_close(mqdes)==-1){
        printf("mq_close error");
        return 1;
    }

  return 0;
}



