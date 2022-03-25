#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#define MAX_LENGTH 500
#define clear() printf("\033[H\033[J")

static char *correctAnswers;
static int numOfQuestions = 0;
static int cnt = 0;

int GetNumOfQuestions(FILE* fp){

    char str[MAX_LENGTH];
    int ret = 0;
   
    while(fgets(str, MAX_LENGTH, fp)){
        
        ret++;
    }

    return ret;
}

void InitArray(FILE* fp){

    int i;

    numOfQuestions = GetNumOfQuestions(fp);
    correctAnswers = malloc(numOfQuestions*(sizeof(char)));

    for(i = 0; i < numOfQuestions; i++)
       correctAnswers[i] = 0;
}

int IsNumInArray(int num){

    return correctAnswers[num];
}

void AddNumToArray(int num){

    correctAnswers[num] = 1;
}

int GenerateRand(){

    int ret = 0;

    do{

        ret = rand() % numOfQuestions;

    } while(IsNumInArray(ret)); 

    return ret;
}

void GetQuestion(FILE* fp, int num, char* str){

    int i;

    for(i = 0; i < num+1; i++)
        fgets(str, MAX_LENGTH, fp);
}

int IsQuestionOk(){ 

    char str[10] = {0};

    printf("Is the answer correct?\n");
    scanf("%s", str); 

    if((str[0] == 'Y' || str[0] == 'y') && str[1] == 'e' && str[2] == 's' && str[3] == 0)
        return 1;
    else if((str[0] == 'N' || str[0] == 'n') && str[1] == 'o' && str[2] == 0) 
        return 0;
    else if(str[0] == 'S' && str[1] == 'T' && str[2] == 'O' && str[3] == 'P' && str[4] == 0)
        return -2;
    else
        return -1;
}

void ReadFromBinFile(FILE* fpb){

    rewind(fpb);
    fread(&cnt, sizeof(cnt), 1, fpb);
    for(int i = 0; i < numOfQuestions; i++)
        fread(&correctAnswers[i], sizeof(correctAnswers[i]), 1, fpb);
}

void WriteToBinFile(FILE* fpb){

    rewind(fpb);
    fwrite(&cnt, sizeof(cnt), 1, fpb);
    for(int i = 0; i < numOfQuestions; i++)
        fwrite(&correctAnswers[i], sizeof(correctAnswers[i]), 1, fpb);
}

void OpenTxtFile(FILE** fp){

    *fp = fopen("questions.txt", "r");

    if(*fp == NULL){

        perror("ERROR!");
        exit(1);
    } 
}

void OpenBinFile(FILE** fpb){

    *fpb = fopen("arr.bin", "rb+");

    if(*fpb == NULL){

        *fpb = fopen("arr.bin", "wb+");

        if(*fpb == NULL){

            perror("ERROR!");
            exit(1);
        }    
    }
    else{

        ReadFromBinFile(*fpb);
    }
}

void FulyWriteToBinFile(FILE* fpb){

    fpb = fopen("arr.bin", "rb+");

    if(fpb == NULL){
        perror("ERROR!");
        exit(1);
    }

    WriteToBinFile(fpb);
    fclose(fpb);
} 

void Flashcards(){
    
    FILE* fp;
    FILE* fpb;
    int questionNum = 0;
    char question[MAX_LENGTH] = {0};
    int ret = 0;

    OpenTxtFile(&fp);
    InitArray(fp);

    OpenBinFile(&fpb);
    fclose(fpb);


    srand(time(0));

    while(cnt != numOfQuestions){

        clear();
        rewind(fp);
        questionNum = GenerateRand();
        GetQuestion(fp, questionNum, question); 
        printf("%s", question);

        while((ret = IsQuestionOk()) == -1){
            
            printf("Wrong answer, try again\n");
        }
        
        if(ret == -2)
            break;

        if(ret == 1){

            AddNumToArray(questionNum);
            cnt++;
            FulyWriteToBinFile(fpb);
        }
    }
    

    clear();
   
    if(ret == -2){

        printf("Not finished yet, grab a snack and continue!\n");
    }
    else{

        printf("Repetition finished!\n");
        printf("Now it's time for rest!\n");
    }

    fclose(fp);
    free(correctAnswers);
}

int main(void){

    
    Flashcards();

    return 0;
}
