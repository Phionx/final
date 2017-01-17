#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>

int main()
{
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(9001);

  int sock = socket(AF_INET, SOCK_STREAM,0);
  connect(sock, (struct sockaddr *) &serv_addr,sizeof(serv_addr));
  char in[256], out[256];

  while(read(sock,in,sizeof(in)))
    {
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
  return 0;
}
