#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

//int shm_fd = -1;
//char* shared_mem;

errorhandle(char* from) {
  printf("\nError in %s: %s.\n", from, strerror(errno));
  exit(errno);
}
/*
void sighandler(int sig) {
  if(sig == SIGINT || sig == SIGTERM || sig == SIGQUIT) {
    if(shm_fd != -1) {
      shmdt(shared_mem);
      shmctl(shm_fd, IPC_RMID, NULL);
      printf("Shared memory deleted.");
    }
    signal(sig, SIG_DFL);
    printf("Reraising sig %s.", strsignal(sig));
    raise(sig);
    exit(sig);
  }
}
*/

int main() {
//  signal(SIGINT, sighandler);
//  signal(SIGTERM, sighandler);
//  signal(SIGQUIT, sighandler);

  int sockdes,newsockdes;
  struct sockaddr_in serv_addr, cli_addr;
  sockdes = socket(AF_INET, SOCK_STREAM, 0);

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(9001);


  if(bind(sockdes, (struct sockaddr *) &serv_addr, sizeof(serv_addr))<0) {
    errorhandle("bind");
  }

  listen(sockdes,5);
  printf("listening for connections\n");
  socklen_t cli_len = sizeof(cli_addr);

  //int msize; // the size (in bytes) of the shared memory segment
  //const char *name = "questions";
  //int key = 123456;
  //shm_fd = shmget(key, 1, IPC_CREAT | 0666);
  //shared_mem = (char* )shmat(shm_fd, NULL, 0);
  int fdinfo[2];
  pipe(fdinfo);
  printf("%d %d\n",fdinfo[0], fdinfo[1]);

    char out[] = "These substances are transported by PIN proteins and bind to TIR1. They stimulate proton pumps to lower the pH and activate expansins, according to the acid growth hypothesis. In high concentrations, they stimulate excess ethylene production, which induces abscission, hence the use of these compounds in herbicides like Agent Orange. Indole-3-acetic acid is one example of these compounds which contribute to apical dominance, phototropisms, and cell elongation. For 10 points, name these plant hormones  whose effect is strengthened in the presence of cytokinins and gibberellins. ";

  char *outte = out;

  char *lol;

  //outte = shared_mem;
  int start = 0;
  if(fork()){
    while(1) {
     
      newsockdes = accept(sockdes, (struct sockaddr *) &cli_addr, &cli_len);
      printf("%d\n",start);
      if(newsockdes == -1)
	errorhandle("accept");
      else {
	write(fdinfo[1],"hi",3);
	if(!fork()) {
	//child
	  char in[256];//, out[];
	  read(fdinfo[0],in,256);
	  write(fdinfo[1],in,strlen(in)+1);
	  printf("found connection from [%d]\n",getpid());
	  while(write(newsockdes, in, strlen(in)+1)){
	    
	    //	printf("shm_mem: %s \n",shared_mem);
	    //     write(newsockdes, in, strlen(lol)+1);
	  //   if(write(newsockdes, in, strlen(lol)+1) == -1)
	  //errorhandle("write");// read(newsockdes, in, 256))
	    read(fdinfo[0],in,256);

	    printf("sending to [%d]:%s \n", getpid(),in);	  
	  }
	  
	  printf("ended\n");
	  printf("[%d] disconnected\n",getpid());
	  close(newsockdes);
	  return 0;
	//break;
	} 
      }
    }
  }
  else{
    while(1){
      char in[256];
      read(fdinfo[0],in,256);
      while(!strcmp(in,"hi")) {
	//parent
	printf("ok\n");
	lol = strsep(&outte," ");
	write(fdinfo[1],lol,strlen(lol)+1);
	
	struct timespec time;
	time.tv_nsec = 300000000;
	time.tv_sec = 0;
	/*
	while(lol) {
	if(write(newsockdes, lol, strlen(lol)+1) == -1)
	errorhandle("write");// read(newsockdes, in, 256))
	printf("sending to [%d]:%s \n", getpid(),lol);
	lol = strsep(&outte," ");
	//      printf("sending to [%d]:%s \n", getpid(),lol);
	// if(!lol)
	//break;*/
	nanosleep(&time, &time);
	//strcpy(shared_mem, outte);
      }
    }
  }
	
	//printf("shm mem: %s\n", shared_mem);
   
  

  printf("how the fuck\n");
  return 0;
}
