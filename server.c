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
#include "data.h"

const int WAITING = 0;
const int PLAYING = 1;
int state;

struct timespec starttime;
struct timespec nowtime;
// delay is currently 300ms
struct timespec delay;
int sockdes;
//int shm_fd = -1;
//char* shared_mem;

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
    // printf("Received SIGUSR1, killing socket and starting questions.");
    state = PLAYING;
    shutdown(sockdes, SHUT_RDWR);
    clock_gettime(CLOCK_REALTIME, &starttime);
    return;
  }
  if(sig == SIGINT) {
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

int send_tick(int sd, char *word) {
  char writeBuf[256];
  if(ansavail) {
    if(write(sd, addHeader(writeBuf, HEADER_WORD, word), 256) == -1)
      return 1;
  }
  return 0;
}

char *receive_tick(int sd) {
  char buf[256];

  if(read(sd, buf, 256) != -1) {
    header head = remHeader(buf);

    if(head == HEADER_ANSWER_REQUEST) {
      if(ansavail) {
        write(sd, addHeader(buf, HEADER_ANSWER_ACCEPT, buf), 256);
        ansavail = 0;
      }
      else {
        write(sd, addHeader(buf, HEADER_ANSWER_DENIED, buf), 256);
      }
    }

    else if(head == HEADER_ANSWER) {
      ansavail = 1;
      char *pbuf = malloc(256);
      strcpy(pbuf, buf);
      return pbuf;
    }
  }
  return 0;
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

sendScores(int *sds, int *scores) {
  int evenScore, oddScore, i;
  evenScore = 0;
  oddScore = 0;
  char evenBuf[256], oddBuf[256];
  for(i = 0; i < 5; i++) {
    evenScore += scores[2 * i];
    oddScore += scores[2 * i + 1];
  }
  sprintf(evenBuf, "%c%d", HEADER_SCORE, evenScore);
  sprintf(oddBuf, "%c%d", HEADER_SCORE, oddScore);
  for(i = 0; i < 5; i++) {
    if(sds[2 * i] != -1)
      write(sds[2 * i], evenBuf, 256);
    if(sds[2 * i + 1] != -1)
      write(sds[2 * i + 1], oddBuf, 256);
  }
}

unsigned long *teamScores(unsigned long *scores) {
  unsigned long *tscores = calloc(2, sizeof(unsigned long));
  int i;
  for(i = 0; i < 5; i++) {
    tscores[0] += scores[2 * i];
    tscores[1] += scores[2 * i + 1];
  }
  return tscores;
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
  socklen_t cli_len = sizeof(cli_addr);

  //int msize; // the size (in bytes) of the shared memory segment
  //const char *name = "questions";
  //int key = 123456;
  game myGame;
  myGame = init(myGame);
  srand(time(NULL));
  int rn = rand() % (myGame.roundNum - 1);
  round myRound = myGame.rounds[rn];
  question *questions = myRound.questions;
  int numQuestions = myRound.numberOfQuestions;
  printf("This round has %d questions.", numQuestions);
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
  int sds[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
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
      printf("Player %d joined team %s.\n", numPlayers, numPlayers % 2 ? "red" : "blue");
      //printf("SD is %d, numPlayers is %d\n", sd, numPlayers);
    }
  }
  else {
    char temp[2];
    fgets(temp, 2, stdin);
    kill(masterPID, SIGUSR1);
    exit(0);  // job done, parent now has all children and can continue
  }
  int i,j,k;
  int scores[] = {0,0,0,0,0,0,0,0,0,0};
  //printf("sample question: %s", questions[5].tossUpQuestion);
  for(j = 0; j < numQuestions; j++) {
    //printf("IN da loop\n");
    int bonus;
    question q = questions[j];
    int winner = -1;
    for(bonus = 0; bonus < 2; bonus++) {
      //printf("bonus loop\n");
      char *sentence;
      char *answer;
      char roundEnded = 0;
      if(!bonus) {
        sentence = malloc(strlen(q.tossUpQuestion) + 1);
        strcpy(sentence, q.tossUpQuestion);
        answer = q.tossUpAnswer;
      }
      else {
        sentence = malloc(strlen(q.bonusQuestion) + 1);
        strcpy(sentence, q.bonusQuestion);
        answer = q.bonusAnswer;
        if(winner != -1) {
          for(i = winner % 2 ? 0 : 1; i < numPlayers; i+=2) {
            sd = sds[i];
            if(sd != -1) {
              char nothing[2] = "";
              write(sd, addHeader(nothing, HEADER_BONUS_WAIT, nothing), 256);
            }
          }
        }
      }
      

      char *word = strsep(&sentence, " ");
      while(word != NULL && !roundEnded) {
        if(ansavail) {
          nanosleep(&delay, NULL);
        }
        for(i = 0; i < numPlayers; i++) {
          sd = sds[i];

          if(sd != -1) {
            //printf("hello\n");
            send_tick(sd, word);
            char *rv = receive_tick(sd);
            //printf("%li", rv);
            if(rv == 1) {
              sds[i] = -1;
            }
            else if(rv) {  // rv is a given answer
              //printf(rv);
              if(checkAnswer(rv, answer)) {
                winner = i;
                //printf("winner: %d", winner);
                scores[i]++;
                sendScores(sds, scores);
                roundEnded = 1;
                break;
              }
            }
            else {
              //printf(rv);
            }
          }
          //printf("shm mem: %s\n", shared_mem);
        }
        if(ansavail)
          word = strsep(&sentence, " ");
      }
      char temp[5] = "";
      for(i = 0; i < numPlayers; i++) {
        int sd = sds[i];
        if(sd != -1) {
          write(sd, addHeader(temp, HEADER_ROUNDEND, temp), 256);
        }
      }
    }
  }
  int *tscores = teamScores(scores);
  char gamewon[10];
  gamewon[0] = HEADER_ENDGAME;
  gamewon[1] = 1;
  gamewon[2] = 0;
  char gamelost[10];
  gamelost[0] = HEADER_ENDGAME;
  gamelost[1] = 0;
  gamelost[2] = 0;
  int sdi;
  if(tscores[0] > tscores[1]) {
    for(sdi = 0; sdi < numPlayers; sdi++) {
      int sd = sds[sdi];
      if(sd != -1) {
        if(sdi % 2 == 0)
          write(sd, gamewon, 256);
        else
          write(sd, gamelost, 256);
      }
    }
  }
  else {
    for(sdi = 0; sdi < numPlayers; sdi++) {
      int sd = sds[sdi];
      if(sd != -1) {
        if(sdi % 2 == 1)
          write(sd, gamewon, 256);
        else
          write(sd, gamelost, 256);
      }
    }
  }
  return 0;
}
