//
// Created by sobol@wincs.cs.bgu.ac.il on 3/13/18.
//
#include<stdio.h>
#include<stdlib.h>
#include <string.h>

#define max(X, Y) (((X) > (Y)) ? (X) : (Y))
int countWords(char c, int* word);
void printRes(int counter[], int argc, char *argv[], char *arg[]);
int main( int argc, char *argv[] ){
    FILE* stream = stdin;
	char c;
    int* word = (int *)malloc(sizeof(int));
    *word = 0;
    int len = 0;
    int counter[4] = {0,0,0,0};
    char *arg[4] ={"-w","-c","-l","-n"};
    for(int i=1;i<argc && stream == stdin;i++)
        if(strcmp(argv[i],"-i") ==0)
            stream = fopen(argv[i + 1], "r");
    c = fgetc(stream);
    while(c!= EOF){
        if(countWords(c,word))
            counter[0] += 1;
        if(*word == 1)
            len ++;
        else {
            counter[2] = max(len, counter[2]);
            len = 0;
        }
        if(!(c <= ' '))
            counter[1] +=1;
        if(c == '\n')
            counter[3]++;
        c = fgetc(stream);
    }
    if(counter[1] > 0)
        counter[3]++;
    if(*word)
        counter[2] = max(len, counter[2]);
    printf("\n");
    printRes(counter,argc,argv,arg);
    free(word);
    if(stream != stdin)
        fclose(stream);
    return 0;
}


int countWords(char c, int* word){
    if(c <= ' ') {
        if (*word == 1) {
            *word = 0;
            return 0;
        }
    }
    else{
        if(*word == 0) {
            *word = 1;
            return 1;
        }
    }
    return 0;
}

void printRes(int counter[], int argc, char *argv[], char *arg[]){
    if(argc == 1 || (argc == 3 && strcmp(argv[1],"-i") ==0))
        printf("%d\n",counter[0]);
    else{
        for(int i = 0;i < 4;i++) {
            for (int j = 1; j < argc; j++) {
                if(strcmp(arg[i] , argv[j]) == 0)
                    printf("%d\n", counter[i]);
            }
        }
    }
}
