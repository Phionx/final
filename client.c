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
enum state_t {
  STATE_DEFAULT,
  STATE_ANSWER,
  STATE_DONE
};
typedef enum state_t state;
state curr = STATE_DEFAULT;
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
  char *formatted = strtok(answer, "\n");
  addHeader(answer, HEADER_ANSWER, formatted);
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
  int infd = fileno(stdin);
  setBlocking(infd, 0);
  setBlocking(sock, 0);
  while(1) {
    if(read(sock,in,256) != -1) {
      header head = remHeader(in);
      if(head == HEADER_WORD)
        printf("%s ", in);
      else if(head == HEADER_ANSWER_ACCEPT) {
        curr = STATE_DONE;
        
        setBlocking(infd, 1);
        sendAnswer();
        setBlocking(infd, 0);
      }
      else if(head == HEADER_ANSWER_DENIED) {
        curr = STATE_DEFAULT;
      }
      else if(head == HEADER_SCORE) {
        printf("\nYour team score: %s\n", in);
      }
      else if(head == HEADER_ROUNDEND) {
        while(read(infd, in, 256) != -1) {};  // clear stdin after roundend
        curr = STATE_DEFAULT;
      }
      else if(head == HEADER_BONUS_WAIT) {
        curr = STATE_DONE;  // sit out this round, it's a bonus
      }
    }
    if(curr == STATE_DEFAULT) {
      if(read(infd, in, 256) != -1) {
        write(sock, addHeader(in, HEADER_ANSWER_REQUEST, ""), 256);
        curr = STATE_ANSWER;
      }
    }
  }
  return 0;
}
