#ifndef CLIENT_H
#define CLIENT_H

#include "comms.h"

int main(int argc,char *argv[]);
void sighandler(int sig);
extern int sock;

#endif