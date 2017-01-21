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
#include <sys/un.h>

int shm_fd = -1;
char* shared_mem;

errorhandle(char* from) {
  printf("\nError in %s: %s.\n", from, strerror(errno));
  exit(errno);
}

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

int main() {
  signal(SIGINT, sighandler);
  signal(SIGTERM, sighandler);
  signal(SIGQUIT, sighandler);

  int sockdes,newsockdes, localsd;
  struct sockaddr_in serv_addr, cli_addr;
  sockdes = socket(AF_INET, SOCK_STREAM, 0);

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(9001);


  if(bind(sockdes, (struct sockaddr *) &serv_addr, sizeof(serv_addr))<0) {
    errorhandle("bind");
  }

  listen(sockdes,10);
  printf("listening for connections\n");
  socklen_t cli_len = sizeof(cli_addr);

  //int msize; // the size (in bytes) of the shared memory segment
  //const char *name = "questions";
  //int key = 123456;
  //shm_fd = shmget(key, 1, IPC_CREAT | 0666);
  //shared_mem = (char* )shmat(shm_fd, NULL, 0);
  // if(access("sock", F_OK) == -1) {
  //   int sdlocal = socket(AF_UNIX, SOCK_STREAM, 0);  // local socket for ipc

  //
  //   if(bind(sdlocal, (struct sockaddr *) &local_addr, sizeof(local_addr)) < 0) {
  //     errorhandle("local bind");
  //   }
  // }
  // int localsock = open("sock", O_RDWR);
  int localsock[2];
  socketpair(PF_LOCAL, SOCK_STREAM, 0, localsock);
  int parentsock = localsock[0];
  int childsock = localsock[1];\

  struct sockaddr_un local_addr;
  local_addr.sun_family = AF_UNIX;
  strcpy(local_addr.sun_path, "sock");

  char out[] = "These substances are transported by PIN proteins and bind to TIR1. They stimulate proton pumps to lower the pH and activate expansins, according to the acid growth hypothesis. In high concentrations, they stimulate excess ethylene production, which induces abscission, hence the use of these compounds in herbicides like Agent Orange. Indole-3-acetic acid is one example of these compounds which contribute to apical dominance, phototropisms, and cell elongation. For 10 points, name these plant hormones  whose effect is strengthened in the presence of cytokinins and gibberellins. ";

  char *outte = out;

  char *lol;

  //outte = shared_mem;
  // int start = 0;
  int f = fork();
  if(!f) {  // this fork runs the accept loop
    close(parentsock);
    while(1) {
      newsockdes = accept(sockdes, (struct sockaddr *) &cli_addr, &cli_len);
      // printf("%d\n",start);
      if(newsockdes == -1)
        errorhandle("accept");
      else {
        // start = 1;
        if(!fork()) {
          connect(childsock, (struct sockaddr *)&local_addr, sizeof(local_addr));
        	//child
        	char in[256];//, out[];
        	//connect(sdlocal, (struct sockaddr *) &serv_addr,sizeof(serv_addr));
        	printf("found connection from [%d]\n",getpid());
        	while(write(newsockdes, in, strlen(in)+1)){
        	  //	printf("shm_mem: %s \n",shared_mem);
        	  //     write(newsockdes, in, strlen(lol)+1);
        	  //   if(write(newsockdes, in, strlen(lol)+1) == -1)
        	  //errorhandle("write");// read(newsockdes, in, 256))
        	  if(read(childsock,in,256) == -1) {
              continue;
            }
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
  else {
    listen(parentsock, 10);
    close(childsock);
    while(1) {
      //parent
      printf("ok\n");
      lol = strsep(&outte," ");
      write(parentsock,lol,strlen(lol)+1);

      struct timespec time;
      time.tv_nsec = 300000000;
      time.tv_sec = 0;
      nanosleep(&time, &time);
    }
  }
      /*
	while(lol) {
	if(write(newsockdes, lol, strlen(lol)+1) == -1)
	  errorhandle("write");// read(newsockdes, in, 256))
	printf("sending to [%d]:%s \n", getpid(),lol);
	lol = strsep(&outte," ");
	//      printf("sending to [%d]:%s \n", getpid(),lol);
	// if(!lol)
	//break;*/
	//strcpy(shared_mem, outte);
	//printf("shm mem: %s\n", shared_mem);

  printf("PID %d ended outside of loop. It is%s forked.\n", getpid(), f ? " not" : "");
  return 0;
}
