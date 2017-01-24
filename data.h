#ifndef DATA_H
#define DATA_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#define ROW_SIZE 6
#define QUESTIONS 10000 //change this CAP on questions from each subject later
#define ROUND_SIZE 26
#define MAX_ROUNDS 100


typedef struct {
	char * subject;
	char * tossUpQuestion;
	char * tossUpAnswer;
	char * bonusQuestion;
	char * bonusAnswer;
} question;


typedef struct {
	int numberOfQuestions;
	question questions [ROUND_SIZE];
} round;

typedef struct {
	int roundNums;
	round rounds [MAX_ROUNDS];
} game;

game init (game scibowl);

#endif