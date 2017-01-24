#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#define MAX_LINE_LENGTH 20000000000
#define ROW_SIZE 6
#define QUESTIONS 10000 //change this CAP on questions from each subject later
#define ROUND_SIZE 26
#define MAX_ROUNDS 1000


typedef struct {
	char * subject;
	char * TossUpQuestion;
	char * TossUpAnswer;
	char * BonusQuestion;
	char * BonusAnswer;
} question;


typedef struct {
	int numberOfQuestions;
	question questions [ROUND_SIZE];
} round;


int main (int argc, char *argv []) {
	char line[MAX_LINE_LENGTH] = {0};
	//round rounds [MAX_ROUNDS];
	question questions [QUESTIONS];
	struct stat st;
	stat("data.csv", &st);
	int size = st.st_size;

	int i = 0;

	FILE *questionsFile = fopen("data.csv", "r");
    if (questionsFile)
    {
        char *token = 0;
		char *innerToken = 0;
        while (fgets(line, size , questionsFile)) 
        {
			token = strtok(&line[0], "⏒");
            while (token)
            {
				innerToken = strtok(token, "⏑");
				while (innerToken){
					
					innerToken = strtok(NULL, "⏑");
				}

				/*temp[tempCurr] = malloc(strlen(token) + 1); //This is where it all goes wron
				strcpy(temp[tempCurr], token);
				tempCurr++;*/
                token = strtok(NULL, "⏒");
            }

        }
        fclose(questionsFile);
    }


	//printf("Testing Parser: %s", CSVArray[1][1]);
	
	
	
	return 0;

}
