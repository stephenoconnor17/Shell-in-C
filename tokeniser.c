#include "tokeniser.h"
#include <stdio.h>
#include <stdlib.h>

int is_delim(char c) {
    return c == ' ' || c == '\n' || c == '\t'; 
}

char** tokenise(char* line){
    int onWord = 0;
    int wordCount = 0;

    int i = 0;
    char current = *(line + i);
    while(current != '\0'){
        if(!is_delim(current) && onWord == 0){
            wordCount++;
            onWord = 1;
        }else if(is_delim(current) && onWord == 1){
            onWord = 0;
        }

        i++;
        current = *(line + i);
    }

    //an dynamic array of dynamic strings.
    //+1 for null to acknowledge end of arr.
    char** arr = malloc(sizeof(char*) * (wordCount + 1));

    i = 0;
    onWord = 0;
    current = *(line + i);
    int j = 0;

    int currentWordLength = 0;

    do{
        if(!is_delim(current) && onWord == 0){
            onWord = 1;
            currentWordLength++;
        }else if((is_delim(current) || current == '\0') && onWord == 1){
            onWord = 0;
            *(arr + j) = malloc(sizeof(char) * (currentWordLength + 1));
            for(int k = 0; k < currentWordLength; k++){
                *(*(arr + j)+k) = *(line + i - currentWordLength + k);
            }
            //h e l p e r
            //0 1 2 3 4 5
            //6
            *(*(arr + j)+currentWordLength) = '\0';
            j++;
            currentWordLength = 0;
        }else if(!is_delim(current)){
            currentWordLength++;
        }
        
        if(j < wordCount){
            i++;
            current = *(line + i);
        }
    }while(j < wordCount);

    *(arr + wordCount) = NULL;

    return arr;
}

void free_tokens(char** args){
    int i = 0;
    while(*(args + i) != NULL){
        free(*(args + i));
        i++;
    }
    free(args);
}