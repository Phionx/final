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
#include <sys/sem.h>
#include "server.h"


const int WAITING = 0;
const int PLAYING = 1;
int state;

struct timespec starttime;
struct timespec nowtime;
// delay is currently 300ms
struct timespec delay;
int semid;
int sockdes;
//int shm_fd = -1;
//char* shared_mem;

union semun {
  int val;
  struct semid_ds *buf;
  ushort *array;
};

int ansavail = 1;

/* struct sembuf { */
/*   ushort_t        sem_num;        /\* semaphore number *\/ */
/*   short           sem_op;         /\* semaphore operation *\/ */
/*   short           sem_flg;        /\* operation flags *\/ */
/* }; */


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
  if(sig == SIGINT) {
    killSem();
    exit(sig);
  }
  /*  if(sig = SIGUSR2)
    {
      state = WAITING;
      kill(getppid(),SIGUSR2);
      return;
      }*/
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

char *tick(int sd, char *word) {
  char readBuf[256];
  char writeBuf[256];

  if(ansavail){
    if(write(sd, word, 256) == -1)
     return 1;
  }
  if(read(sd, readBuf, 256) != -1) {
    header head = remHeader(readBuf);
    if(head == HEADER_INTERRUPT) {
      printf("Header interrupt\n");
      return 1;
    }

    else if(head == HEADER_ANSWER_REQUEST && ansavail) {
      write(sd, addHeader(readBuf, HEADER_ANSWER_ACCEPT, readBuf), 256);
      ansavail = 0;
	  }

    else if(head == HEADER_ANSWER) {
      printf("answer: %s\n", readBuf);
      ansavail = 1;
      char *buf = malloc(256);
      strcpy(buf, readBuf);
      return buf;
    }
  }
  return 0;
}

killSem() {  // program control is not returned
  char semidStr[20];
  sprintf(semidStr, "%d", semid);
  execlp("ipcrm", "ipcrm", "-s", semidStr, 0);
}

char checkAnswer(char *answer, char *realAnswer) {
  printf("\n\nUser answered: %s\n", answer);
  printf("Real answer: %s\n", realAnswer);
  printf("Is this correct? (y/n) ");
  char correct[3];
  fgets(correct, 3, stdin);
  if(!strcmp(correct, "y\n"))
    return 1;
  else if(!strcmp(correct, "n\n"))
    return 0;
  else
    return checkAnswer(answer, realAnswer);
}


int main(int argc, char *argv[]) {
//  signal(SIGINT, sighandler);
//  signal(SIGTERM, sighandler);
//  signal(SIGQUIT, sighandler);

  setbuf(stdout, NULL);
  signal(SIGUSR1, sighandler);
  signal(SIGINT, sighandler);
  int newsockdes;

  struct sockaddr_in serv_addr, cli_addr;
  sockdes = socket(AF_INET, SOCK_STREAM, 0);

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(7001);


  if (bind(sockdes, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    errorhandle("bind");
  }

  listen(sockdes,10);
  printf("listening for connections\n");
  socklen_t cli_len = sizeof(cli_addr);

  //int msize; // the size (in bytes) of the shared memory segment
  //const char *name = "questions";
  //int key = 123456;

  char out[] = "These substances are transported by PIN proteins and bind to TIR1. They stimulate proton pumps to lower the pH and activate expansins, according to the acid growth hypothesis. In high concentrations, they stimulate excess ethylene production, which induces abscission, hence the use of these compounds in herbicides like Agent Orange. Indole-3-acetic acid is one example of these compounds which contribute to apical dominance, phototropisms, and cell elongation. For 10 points, name these plant hormones  whose effect is strengthened in the presence of cytokinins and gibberellins. \n Unlike the Einstein model, the Debye model properly models how this quantity changes for a substance as temperature decreases, though for higher temperatures it may be derived from a crystal's lattice vibrations through the Law of Dulong and Petit. For a monatomic ideal gas, it is three halves times the ideal gas constant, while that factor is seven-halves for a diatomic ideal gas. For 10 points, identify this quantity, the amount of energy necessary to increase the temperature of a unit quantity of a substance by a unit amount. ";
  char answer[] = "answer";
  // temp, later we will read from file
  char *outte = out;

  char *lol;

  semid = semget(123456,1,0666 | IPC_CREAT | IPC_EXCL);// | S_IRUSR |
  //		 S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
  union semun arg;
  arg.val = 1;
  semctl(semid,0,SETVAL,arg);
  printf("sem val: %d\n",semctl(semid,0,GETVAL));
  //  union semun arg;

  //outte = shared_mem;
  delay.tv_nsec = 300000000;
  delay.tv_sec = 0;
  state = WAITING;
  int start = 0;
  int children[10];
  int numPlayers = 0;
  printf("Waiting for players. Press enter when all players have joined.\n");
  int masterPID = getpid();
  int f = fork();  // parent gets new clients, child waits for input
  int isMaster = 1;
  int sd = -1;
  int sds[10];
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
      sds[numPlayers++] = sd;
      setBlocking(sd, 0);
      printf("Player %d joined.\n", numPlayers);
    }
  }
  else {
    char temp[2];
    fgets(temp, 2, stdin);
    kill(masterPID, SIGUSR1);
    printf("Master has been sent SIGUSR1\n");
    exit(0);  // job done, parent now has all children and can continue
  }
  printf("Out of loop\n");
  int i;
  char *word = strsep(&outte, " ");
  char headerWord[256];
  int scores[] = {0,0,0,0,0,0,0,0,0,0};
  semid = semget(123456,0,0);
  while(outte && word != NULL) {
    if(ansavail) {
      printf("Sending %s", word);
      addHeader(headerWord, HEADER_WORD, word);
      nanosleep(&delay, NULL);
      for(i = 0; i < numPlayers; i++) {
        sd = sds[i];
        if(sd != -1) {
          char *rv = tick(sd, headerWord);
          printf("%li", rv);
          if(rv == 1) {
            sds[i] = -1;
          }
          else if(rv) {  // rv is a given answer
            printf(rv);
            if(checkAnswer(rv, answer))
              scores[i]++;
          }
          else {
            printf(rv);
          }
        }
        //printf("shm mem: %s\n", shared_mem);
      }
      word = strsep(&outte, " ");
    }
  }
  printf("PID %d ended outside of loop. It is%s forked.\n", getpid(), f ? " not" : "");
  killSem();
  return 0;
}
