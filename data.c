#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#define ROW_SIZE 6
#define QUESTIONS 10000 //change this CAP on questions from each subject later
#define ROUND_SIZE 26
#define MAX_ROUNDS 1000


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


int main (int argc, char *argv []) {
	char * line;
	//round rounds [MAX_ROUNDS];
	question questions [QUESTIONS];
	/*
	struct stat st;
	stat("data.csv", &st);
	int size = st.st_size;
	printf("\nsize of file: %d", size);
	printf("\nlenght of line before it was filled: %d\n", (int)strlen(line));*/
	int i = 0;
	int j = 0;

	FILE *questionsFile = fopen("data.csv", "rb");
    if (questionsFile)
    {
        //fgets(line, size , questionsFile);
		fseek(questionsFile, 0, SEEK_END);
		int length = ftell(questionsFile);
		printf("\nlenght of csv: %d", length);
		fseek(questionsFile, 0, SEEK_SET);
		line = malloc(length);
		if (line){
			fread(line, 1, length, questionsFile);
		}
		fclose (questionsFile);
	}

	//printf("%s", line);

	
        char *token = 0;
		char *innerToken = 0;
		//printf("\n%s\n", line);

		printf("\nlenght of line after it was filled: %d\n", (int)strlen(line));
		
		i = 0;
		
		//char * test = "a⏑b⏑c⏒d⏑e⏑f⏒g⏑e⏒";
		while ( (token = strsep(&line, "\x02")) != NULL){
				//printf("\ntoken: %s", token);
				char * foo = strdup(token);
				while ((innerToken = strsep(&foo, "\x03")) != NULL){
					char * goo = strdup(innerToken);
					if (strlen(goo) > 0) {
					if (goo[0] == '0') {
						printf("\nSubject innertoken: %s", goo + 3);
						questions[i].subject = (char *)malloc(sizeof(goo) + 1);
						strcpy(questions[i].subject, goo + 3);
					} else if (goo[0] == '1') {
						printf("\nToss up question innertoken: %s", goo + 3);
						questions[i].tossUpQuestion = (char *)malloc(sizeof(goo) + 1);
						strcpy(questions[i].tossUpQuestion, goo + 3);
					} else if (goo[0] == '2') {
						printf("\nToss up ans innertoken: %s", goo + 3);
						questions[i].tossUpAnswer = (char *)malloc(sizeof(goo) + 1);
						strcpy(questions[i].tossUpAnswer, goo + 3);
					} else if (goo[0] == '3') {
						printf("\nBonus question innertoken: %s", goo + 3);
						questions[i].bonusQuestion = (char *)malloc(sizeof(goo) + 1);
						strcpy(questions[i].bonusQuestion, goo + 3);
					} else if (innerToken[0] == '4') {
						printf("\nBonus ans innertoken: %s", goo + 3);
						questions[i].bonusAnswer = (char *)malloc(sizeof(goo) + 1);
						strcpy(questions[i].bonusAnswer, goo + 3);
					}}
				}
				i++;

				/*temp[tempCurr] = malloc(strlen(token) + 1); //This is where it all goes wron
				strcpy(temp[tempCurr], token);
				tempCurr++;*/
            }
		printf("\ni: %d", i); 


		printf("\nTESTING DATA\n");
		printf("\nSubject: %s", questions[0].subject);
		printf("\nToss Up Question: %s", questions[0].tossUpQuestion);
		printf("\nToss Up Answer: %s", questions[0].tossUpAnswer);
		printf("\nBonus Question: %s", questions[0].bonusQuestion);
		printf("\nBonus Ans: %s", questions[0].bonusAnswer);





        
    


	//printf("Testing Parser: %s", CSVArray[1][1]);
	
	
	
	return 0;

}
