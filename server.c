#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

int main()
{
  int sockdes,newsockdes;

  struct sockaddr_in serv_addr, cli_addr;
  sockdes = socket(AF_INET, SOCK_STREAM, 0);

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(5371);


  if(bind(sockdes, (struct sockaddr *) &serv_addr, sizeof(serv_addr))<0)
    {
      printf("BINDING ERROR\n");
    }
  listen(sockdes,5);
  printf("listening for connections\n");
  socklen_t cli_len = sizeof(cli_addr);
 while(1){
    newsockdes = accept(sockdes, (struct sockaddr *) &cli_addr, &cli_len);
    if (newsockdes < 0) 
      printf("ERROR on accept\n");
    if(newsockdes!=-1)
      {
	int pid = fork();
	if(!pid)
	  break;
      }
 }

  printf("found connection from [%d]\n",getpid());
  char buffer[256];
  while(read(newsockdes,buffer,sizeof(buffer))){
    //      char buffer[256];
    //      read(newsockdes,buffer,sizeof(buffer));
      /*if(!strcmp(buffer,"exit\n"))
	{
	  close(newsockdes);
	  
	  return 0;
	  }*/
      printf("message from [%d]: %s",getpid(),buffer);
     }
  printf("[%d] disconnected\n",getpid());
  return 0;
}
