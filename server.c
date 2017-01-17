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

errorhandle(char* from) {
  printf("\nError in %s: %s.\n", from, strerror(errno));
  exit(errno);
}

int main() {
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

  int shm_fd;
  char* shared_mem;
  //int msize; // the size (in bytes) of the shared memory segment
  //const char *name = "questions";
  int key = 123456;
  shm_fd = shmget(key, 1, IPC_CREAT | 0666);
  shared_mem = (char* )shmat(shm_fd, NULL, 0);



  //outte = shared_mem;

 while(1) {
  newsockdes = accept(sockdes, (struct sockaddr *) &cli_addr, &cli_len);
  if(newsockdes < 0)
    errorhandle("accept");
  if(newsockdes!=-1) {
    if(!fork()) {
      //      printf("shm mem: %s\n", shared_mem);
      //      printf("outte: %s\n",outte);
      //    if(shared_mem)
      //strcpy(outte, shared_mem);
      break;
    }
  }
}

 //child process
  printf("found connection from [%d]\n",getpid());
  char in[256];//, out[];

  shm_fd = shmget(123456, 1, 0666);
  shared_mem = (char *) shmat(shm_fd, NULL, 0);
  printf("shm_mem: %s \n",shared_mem);

  char out[] = "These substances are transported by PIN proteins and bind to TIR1. They stimulate proton pumps to lower the pH and activate expansins, according to the acid growth hypothesis. In high concentrations, they stimulate excess ethylene production, which induces abscission, hence the use of these compounds in herbicides like Agent Orange. Indole-3-acetic acid is one example of these compounds which contribute to apical dominance, phototropisms, and cell elongation. For 10 points, name these plant hormones  whose effect is strengthened in the presence of cytokinins and gibberellins. ";

  char *outte = out;
  /*
  if(shared_mem){
    strcpy(outte,shared_mem);
  }
*/
  printf("%s\n",outte);
  char *lol = strsep(&outte," ");
  //  printf("lol: %s\n",lol);

  struct timespec time;
  time.tv_nsec = 300000000;

  while(lol) {
    write(newsockdes, lol, strlen(lol)+1);// read(newsockdes, in, 256))
    printf("sending to [%d]:%s \n", getpid(),lol);
    lol = strsep(&outte," ");
    //      printf("sending to [%d]:%s \n", getpid(),lol);
    // if(!lol)
    //break;
    nanosleep(&time, &time);
    //strcpy(shared_mem, outte);
    //printf("shm mem: %s\n", shared_mem);
  }
  shmdt(shared_mem);
  shmctl(shm_fd, IPC_RMID, NULL);
  //fflush(stdout);
  printf("ended\n");
  printf("[%d] disconnected\n",getpid());
  close(newsockdes);
  return 0;
}
