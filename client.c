#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>

int main() {
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(9001);

  int sock = socket(AF_INET, SOCK_STREAM,0);
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
    fgets(temp, 2, stdin);
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
