#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>//not available in windows so using WSL ubuntu with vscode using c/c++ extension for intellisense.
#include <linux/limits.h>//this gives us path_max
#include <string.h>
#include "tokeniser.h"

//The main loop of a shell is
/*
1. loop prompting user input
2. grab user input
    2.1 tokenise it into an argv array.
    2.2 from argv[0](the command itself), determine if its external or internal.
3. run command
    3.1 if internal, function defined locally. we own it.
    3.2 if external then
        3.3 first fork the process from unistd.h. this creates a copy of the shell process
        3.4 make sure the return value of the fork is saved in an pid_t because we check if its 0 and resultingly will use
            execvp which replaces the process.
        3.5 the if that checks if fork doesnt return 0 and isnt -1(failure) will use waitpid.

the main difference between internal and external is if the command will affect the shell itself
or more so something the child cant do because it needs to happen to our shell
so "cd" would be internal, so would "exit", but "ls" not internal because it just needs a copy of the directory to list the contents.
*/

int main(){

    char buff[PATH_MAX];
    getcwd(buff, sizeof(buff));
    printf("%s$", buff);
    
    //loop input.
    char* line = NULL;
    char** args = NULL;
    size_t length = 0;


    getline(&line, &length, stdin);
    args = tokenise(line);

    if(*(args + 0) != NULL){
        printf("The command is %s\n", (*(args+0)));
        int i = 1;
        printf("With the flags\n");
        while(*(args + i) != NULL){
            printf("%s\n", *(args+i));
            i++;
        }
    }
    //printf("\n");
    
    free_tokens(args);
    return 0;
}