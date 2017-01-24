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
#include "client.h"


int sock;

void sighandler(int sig) {
  if(sig == SIGINT) {
    write(sock, "\x04", 2);
    exit(SIGINT);
  }
}

sendAnswer() {
  char answer[256];
  printf("Enter the answer: ");
  fgets(answer, 256, stdin);
  printf("You entered: %s\n", answer);
  addHeader(answer, HEADER_ANSWER, strtok(answer, "\n"));
  printf("sending %s", answer);
  write(sock, answer, 256);
}

int main(int argc, char *argv[]) {
  if(argc < 2) {
    printf("USAGE: ./client.out <IP>\n");
    return 1;
  }
  setbuf(stdout, NULL);
  char *ip = argv[1];
  signal(SIGINT, sighandler);
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  inet_aton(ip, &serv_addr.sin_addr);
  serv_addr.sin_port = htons(7001);

  sock = socket(AF_INET, SOCK_STREAM,0);
  connect(sock, (struct sockaddr *) &serv_addr,sizeof(serv_addr));
  char in[256] = "";
  char out[256] = "";
  int f = fork();
  if(!f) {  // child; continues reading until parent sends stop
    while(read(sock,in,256)) {
      header head = remHeader(in);
      if(head == HEADER_WORD)
        printf("%s ", in);
      else if(head == HEADER_ANSWER_ACCEPT) {
        sendAnswer();
      }
    }
  }
  else {  // parent; waits for enter to stop child reading
    char temp[256];
    while(1) {
      fgets(temp, 2, stdin);
      write(sock, addHeader(temp, HEADER_ANSWER_REQUEST, ""), 256);
    }
  }
  return 0;
}
