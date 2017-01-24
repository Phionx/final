#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>
#include <sys/sem.h>
#include <arpa/inet.h>
#include <stdlib.h>

int sock;

void sighandler(int sig) {
  if(sig == SIGINT) {
    write(sock, "\x04", 2);
    exit(SIGINT);
  }
}

int main(int argc, char *argv[]) {
  if(argc < 2) {
    printf("USAGE: ./client.out <IP>\n");
    return 1;
  }
  char *ip = argv[1];
  signal(SIGINT, sighandler);
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  inet_aton(ip, &serv_addr.sin_addr);
  serv_addr.sin_port = htons(7001);

  sock = socket(AF_INET, SOCK_STREAM,0);
  connect(sock, (struct sockaddr *) &serv_addr,sizeof(serv_addr));
  char in[256], out[256];
  int f = fork();
  if(!f) {  // child; continues reading until parent sends stop
    while(read(sock,in,sizeof(in))) {
      //      char *lmao = in;
      //printf("type message: ");
      //    fgets(out,sizeof(out),stdin);
      //write(sock,out,sizeof(out));
      //printf("receiving ");
      //if(read(sock,in,256))
      //      printf("fuck this\n");
      fflush(stdout);
      printf("%s ",in);
      //sleep(1);
      //char in[256];
    }
  }
  else {  // parent; waits for enter to stop child reading
    char temp[256];
    //    int semid = semget(123456,0,0);

    fgets(temp, 2, stdin);
    /*
    struct sembuf ops;
    ops.sem_num = 1;
    ops.sem_op = -1;
    ops.sem_flg = IPC_NOWAIT;
    semop(semid,&ops,1);
    printf("sem val: %d\n",semctl(semid,0,GETVAL));
    */
    kill(f, 9);
    write(sock, "\x02", 2);
    read(sock, temp, 256);  // clear last answer
    char answer[256];
    printf("Enter the answer: ");
    fgets(answer, 256, stdin);
    write(sock, strtok(answer, "\n"), 256);
    read(sock, temp, 2);
    while(temp[0] > 1)
      read(sock, temp, 2);  // make sure that we have correct output
    printf("temp[0]: %d = %c, temp[1]: %d = %c", temp[0], temp[0], temp[1], temp[1]);
    printf("Answer was %s\n", temp[0] ? "correct" : "wrong");
  }
  return 0;
}
