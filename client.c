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
  int sock;
  struct sockaddr_in serv_addr;

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(5371);
  
  sock = socket(AF_INET, SOCK_STREAM,0);
  connect(sock, (struct sockaddr *) &serv_addr,sizeof(serv_addr));  
 while(1)
  {
    
    char buffer[256];
    printf("type message: ");
    fgets(buffer,sizeof(buffer),stdin);
    write(sock,buffer,sizeof(buffer));
    /*  if(!strcmp(buffer,"exit\n"))
    {
	return 0;
	}*/
  }
  return 0;
}
