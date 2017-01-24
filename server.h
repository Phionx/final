#include "comms.h"

int tick_send(int sd, char *word);
char *tick_receive(int sd);
int main(int argc, char *argv[]);
void sighandler(int sig);
int timePassed(struct timespec start, float amtTime);

extern int sockdes;
extern int semid;
extern struct timespec delay;
extern struct timespec nowtime;
extern struct timespec starttime;
extern int state;
extern const int PLAYING;
extern const int WAITING;
