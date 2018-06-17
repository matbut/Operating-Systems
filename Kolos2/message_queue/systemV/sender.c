#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define KEY "./queuekey"

struct msgbuf {
    long mtype;
    char mtext[80];
};


int main(int argc, char* argv[])
{

  if(argc !=2){
    printf("Not a suitable number of program parameters\n");
    return(1);
  }

 /******************************************************
 Utworz kolejke komunikatow systemu V "reprezentowana" przez KEY
 Wyslij do niej wartosc przekazana jako parametr wywolania programu
 Obowiazuja funkcje systemu V
 ******************************************************/

  key_t key=ftok(KEY,'A');
  int msqid;
  if((msqid=msgget(key,IPC_CREAT | 0666))==-1){
      printf("msgget error");
      return 1;
  }

  struct msgbuf msg;
  sprintf(msg.mtext,"%s",argv[1]);
  msg.mtype = 1;


  if (msgsnd(msqid,&msg, sizeof(msg.mtext),0) == -1) {
      printf("msgsnd error");
      return 1;
  }
  printf("sent: %s\n", msg.mtext);

  return 0;
}



