#ifndef COMMS_H
#define COMMS_H

enum header_t {
  HEADER_SCORE = 2,
  HEADER_WORD,
  HEADER_ANSWER_REQUEST,
  HEADER_ANSWER_ACCEPT,
  HEADER_ANSWER,
  HEADER_ANSWER_DENIED,
  HEADER_ROUNDEND,
  HEADER_BONUS_WAIT
};
typedef enum header_t header;

typedef struct {
	char * subject;
	char * tossUpQuestion;
	char * tossUpAnswer;
	char * bonusQuestion;
	char * bonusAnswer;
} question;

char *addHeader(char *dest, header head, char *data);
header remHeader(char *data);
setBlocking(int, int);

#endif