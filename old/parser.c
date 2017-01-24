#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 2048
#define ROW_SIZE 6
#define QUESTIONS 1000 //change this CAP on questions from each subject later
#define ROUND_SIZE 26

typedef struct {
	int numberOfQuestions;
	char * questions[ROUND_SIZE][ROW_SIZE];
} round;

int questionType (char * subject){
	int ans = -1;
	if (strcmp(subject, "Physics")) {
		ans = 0;
	} else if (strcmp(subject, "Chemistry")) {
		ans = 1;
	} else if (strcmp(subject, "Mathematics")) {
		ans = 2;
	} else if (strcmp(subject, "Earth and Space Science")) {
		ans = 3;
	} else if (strcmp(subject, "Energy")) {
		ans = 4;
	} else if (strcmp(subject, "Biology")) {
		ans = 5;
	} else {
		printf("Subject not found");
	}
	return ans;
}

int main (int argc, char *argv []) {
	char line[MAX_LINE_LENGTH] = {0};
    int questionPart = 0; //Part of question being read
    int questionNum = 0; //Question number currently being collected
	char * Data[6][QUESTIONS][ROW_SIZE];
    /*
	 * Data[0] --> Physics
	 * Data[1] --> Mathematics
	 * Data[2] --> Biology
	 * Data[3] --> Chemistry
	 * Data[4] --> EarthSpace
	 * Data[5] --> Energy
	 */

	//total number of each subject questions
	float totalNum;
	int physicsNum;
	int mathematicsNum;
	int biologyNum;
	int chemistryNum;
	int earthSpaceNum;
	int energyNum;


	//current question number on the subject
	int physicsCurr = 0;
	int mathematicsCurr = 0;
	int biologyCurr = 0;
	int chemistryCurr = 0;
	int earthSpaceCurr = 0;
	int energyCurr = 0;
	int tempCurr = 0;
	int i;

	
	FILE *questionsFile = fopen("data.csv", "r");
	char *temp[ROW_SIZE] = {NULL};
    if (questionsFile)
    {
        char *token = 0;
        while (fgets(line, MAX_LINE_LENGTH, questionsFile)) 
        {
			tempCurr = 0;  
			token = strtok(&line[0], "\u0001");
            while (token)
            {
                temp[tempCurr] = malloc(strlen(token) + 1); //This is where it all goes wron
				strcpy(temp[tempCurr], token);
				tempCurr++;
                token = strtok(NULL, "\u0001");
            }
			//adding temp to Data
			switch(questionType(temp[0])){
				case 0:
					for(i=0; i < ROW_SIZE; i++){
						Data[0][physicsCurr][i] = malloc(strlen(temp[i]) + 1);
						strcpy(Data[0][physicsCurr][i], temp[i]);
					}
					physicsCurr++;
					break;	
				case 1:
					for(i=0; i <= ROW_SIZE; i++){
						Data[1][mathematicsCurr][i] = malloc(strlen(temp[i]) + 1);
						strcpy(Data[1][mathematicsCurr][i], temp[i]);
					}
					mathematicsCurr++;
					break;	
				case 2:
					for(i=0; i <= ROW_SIZE; i++){
						Data[2][biologyCurr][i] = malloc(strlen(temp[i]) + 1);
						strcpy(Data[2][biologyCurr][i], temp[i]);
					}
					biologyCurr++;
					break;	
				case 3:
					for(i=0; i <= ROW_SIZE; i++){
						Data[3][chemistryCurr][i] = malloc(strlen(temp[i]) + 1);
						strcpy(Data[3][chemistryCurr][i], temp[i]);
					}
					chemistryCurr++;
					break;	
				case 4:
					for(i=0; i <= ROW_SIZE; i++){
						Data[4][earthSpaceCurr][i] = malloc(strlen(temp[i]) + 1);
						strcpy(Data[4][earthSpaceCurr][i], temp[i]);
					}
					earthSpaceCurr++;
					break;	
				case 5:
					for(i=0; i <= ROW_SIZE; i++){
						Data[5][energyCurr][i] = malloc(strlen(temp[i]) + 1);
						strcpy(Data[5][energyCurr][i], temp[i]);
					}
					energyCurr++;
					break;
				default:
					printf("\nERROR: something went wrong in the subject area\n");
					break;
			}

        }
        fclose(questionsFile);
    }

	totalNum = physicsCurr + mathematicsCurr + biologyCurr + chemistryCurr + earthSpaceCurr + energyCurr;
	physicsNum = (int)(25*physicsCurr/totalNum + .5);
	mathematicsNum = (int)(25*mathematicsCurr/totalNum + .5);
	biologyNum = (int)(25*biologyCurr/totalNum + .5);
	chemistryNum = (int)(25*chemistryCurr/totalNum + .5);
	earthSpaceNum = (int)(25*earthSpaceCurr/totalNum + .5);
	energyNum = (int)(25*energyCurr/totalNum + .5);

	int extra = physicsNum + mathematicsNum + biologyNum + chemistryNum + earthSpaceNum + energyNum - 25;
	earthSpaceNum = earthSpaceNum - extra;

	int num = 1; //special number that tells us which subjects still have
	int primes[] = {2,3,5,7,11,13};
	if (physicsNum > 0) num *= 2;
	if (mathematicsNum > 0) num *= 3;
	if (biologyNum > 0) num *= 5;
	if (chemistryNum > 0) num *= 7;
	if (earthSpaceNum > 0) num *= 11;
	if (energyNum > 0) num *= 13;

	int roundNum = (int)(totalNum/25); //number of rounds to create

	char * subjectNames [] = {"PHYSICS", "MATHEMATICS", "BIOLOGY", "CHEMISTRY", "EARTH and SPACE", "ENERGY"};
	
	int extraQuestions[] = {physicsCurr - physicsNum*roundNum, mathematicsCurr - mathematicsNum*roundNum, biologyCurr - biologyNum*roundNum, chemistryCurr - chemistryNum*roundNum, earthSpaceCurr - earthSpaceNum*roundNum, energyCurr - energyNum*roundNum}; //spare questions or lack there of

	//printf("Testing Parser: %s", CSVArray[1][1]);
	
	
	
	return 0;

}
