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

	printf("%s", line);

	
        char *token = 0;
		char *innerToken = 0;
		//printf("\n%s\n", line);

		printf("\nlenght of line after it was filled: %d\n", (int)strlen(line));
			i = 0;
			token = strtok(line, "⏒");
            while (token != NULL)
            {
				printf("\n testing token: %s", token);
				innerToken = strtok(token, "⏑");
				j = 0;
				while (innerToken){
					if (j == 0) {
						questions[i].subject = (char *)malloc(sizeof(innerToken) + 1);
						strcpy(questions[i].subject, innerToken);
					} else if (j == 1) {
						questions[i].tossUpQuestion = (char *)malloc(sizeof(innerToken) + 1);
						strcpy(questions[i].tossUpQuestion, innerToken);
					} else if (j == 2) {
						questions[i].tossUpAnswer = (char *)malloc(sizeof(innerToken) + 1);
						strcpy(questions[i].tossUpAnswer, innerToken);
					} else if (j == 3) {
						questions[i].bonusQuestion = (char *)malloc(sizeof(innerToken) + 1);
						strcpy(questions[i].bonusQuestion, innerToken);
					} else if (j == 4) {
						questions[i].bonusAnswer = (char *)malloc(sizeof(innerToken) + 1);
						strcpy(questions[i].bonusAnswer, innerToken);
					} else {
						printf("\nERROR in filling up data");
					}
					printf("\nfilling: %s", innerToken);
					innerToken = strtok(NULL, "⏑");
					printf("\nfilling next: %s", innerToken);
					j++;
				}
				i++;

				/*temp[tempCurr] = malloc(strlen(token) + 1); //This is where it all goes wron
				strcpy(temp[tempCurr], token);
				tempCurr++;*/
                token = strtok(NULL, "⏒");
				printf("new token: %s", token);
            }
        
    


	//printf("Testing Parser: %s", CSVArray[1][1]);
	
	
	
	return 0;

}
