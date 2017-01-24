#ifndef SERVER_H
#define SERVER_H

#include "comms.h"

int send_tick(int sd, char *word);
char *receive_tick(int sd);
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

#endif