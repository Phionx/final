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

const int WAITING = 0;
const int PLAYING = 1;
int state;

struct timespec starttime;
struct timespec nowtime;
// delay is currently 10ms
// struct timespec delay;

int sockdes;
//int shm_fd = -1;
//char* shared_mem;

errorhandle(char* from) {
  printf("\nError in %s: %s.\n", from, strerror(errno));
  exit(errno);
}


void sighandler(int sig) {
  if(sig == SIGUSR1) {
    printf("Received SIGUSR1, killing socket and starting questions.");
    state = PLAYING;
    shutdown(sockdes, SHUT_RDWR);
    clock_gettime(CLOCK_REALTIME, &starttime);
    return;
  }
}

/* Returns if amtTime (in ms) has passed since start. */
int timePassed(struct timespec start, float amtTime) {
  clock_gettime(CLOCK_REALTIME, &nowtime);
  return nowtime.tv_sec * 1e9 + nowtime.tv_nsec - start.tv_sec * 1e9 - start.tv_nsec >= amtTime * 1e6;
}

runParent(int nChild, int *children) {
  for(; nChild > 0; nChild--) {
    kill(children[nChild - 1], SIGUSR1);
  }
}

runChild(int sd, char **question) {  // must be ** for strsep
  int amtTime = 1000;
  char *word;
  printf("Running child...\n");
  while(NULL != (word = strsep(question, " "))) {
    while(!timePassed(starttime, amtTime)) {
      sleep(0.01);
    }
    amtTime += 300;  // 300 ms between each word
    printf("Sending child word %s...", word);
    if(write(sd, word, 256) == -1)
      break;
  }
}

int main() {
//  signal(SIGINT, sighandler);
//  signal(SIGTERM, sighandler);
//  signal(SIGQUIT, sighandler);
  setbuf(stdout, NULL);
  signal(SIGUSR1, sighandler);

  int newsockdes;

  struct sockaddr_in serv_addr, cli_addr;
  sockdes = socket(AF_INET, SOCK_STREAM, 0);

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(9001);


  if (bind(sockdes, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
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

  char out[] = "These substances are transported by PIN proteins and bind to TIR1. They stimulate proton pumps to lower the pH and activate expansins, according to the acid growth hypothesis. In high concentrations, they stimulate excess ethylene production, which induces abscission, hence the use of these compounds in herbicides like Agent Orange. Indole-3-acetic acid is one example of these compounds which contribute to apical dominance, phototropisms, and cell elongation. For 10 points, name these plant hormones  whose effect is strengthened in the presence of cytokinins and gibberellins. ";
  // temp, later we will read from file
  char *outte = out;

  char *lol;

  //outte = shared_mem;
  // delay.tv_nsec = 10000000;
  // delay.tv_sec = 0;
  state = WAITING;
  int start = 0;
  int children[10];
  int numPlayers = 0;
  printf("Waiting for players. Press enter when all players have joined.\n");
  int masterPID = getpid();
  int f = fork();  // parent gets new clients, child waits for input
  int isMaster = 1;
  int sd = -1;
  if(f) {
    while (1) {

      if(-1 == (sd = accept(sockdes, (struct sockaddr *) &cli_addr, &cli_len))) {
        if(state == PLAYING) {
          printf("Starting game\n");
          break;
        }
        else {
          printf("Failed accept\n");
          exit(1);
        }
      }
      printf("%d\n", start);
      int childpid = fork();
      if (childpid) {  // parent
        children[numPlayers++] = childpid;
        printf("%dth player joined.\n", numPlayers);
      }
      else {  //child
        isMaster = 0;
        printf("Waiting for game start signal...\n");
        pause();  // wait for SIGUSR1
        printf("Game is starting.\n");
        break;
        }
      
    }
  }
  else {
    char temp[2];
    fgets(temp, 2, stdin);
    kill(masterPID, SIGUSR1);
    printf("Master has been sent SIGUSR1\n");
    exit(0);  // job done, parent now has all children and can continue

  }
  
  if(isMaster)
    runParent(numPlayers, children);
  else
    runChild(sd, &outte);
  //printf("shm mem: %s\n", shared_mem);

  printf("PID %d ended outside of loop. It is%s forked.\n", getpid(), f ? " not" : "");
  return 0;
}
