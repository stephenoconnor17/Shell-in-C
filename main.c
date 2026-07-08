#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>//not available in windows so using WSL ubuntu with vscode using c/c++ extension for intellisense.
#include <linux/limits.h>//this gives us path_max
#include <string.h>
#include "tokeniser.h"
#include <sys/wait.h>

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
   
    //loop input.
    char* line = NULL;
    char** args = NULL;
    size_t length = 0;
    int lastStatus = 0;

    while(1){
        if(getcwd(buff, sizeof(buff)) != NULL){
            printf("%s$", buff);
        }else{
            printf("$\n");
        }

        if(getline(&line, &length, stdin) == -1) break; //didnt have a command returned.

        if((args = tokenise(line)) == NULL){
            printf("ERROR parsing command\n");
            continue;//continue here so we dont free_tokens a null 
        }else if(*(args + 0) != NULL){
            if(strcmp("exit", *(args+0)) == 0){
                free_tokens(args);
                free(line);
                exit(0);
            }else if(strcmp("cd", *(args+0)) == 0){
                if(*(args+1) == NULL){
                    if(chdir(getenv("HOME")) == -1) perror("cd");
                }else{
                    if(chdir(*(args+1)) == -1) perror("cd");
                }
                
            }else{
                pid_t pid = fork();
                if(pid == 0){//child branch, execvp here.
                    if(execvp(*(args+0),args) == -1){//execvp only returns -1 on fails, doesnt return anything at all if it doesnt.
                        perror(*(args + 0));
                        free_tokens(args);
                        free(line);
                        exit(1);
                    }
                }else if(pid == -1){//failure, perror.
                    perror(*(args + 0));
                }else{//positive number, shell branch. waitpid here.
                    int status = 0;
                    waitpid(pid,&status,0);
                    if(WIFEXITED(status)) lastStatus = WEXITSTATUS(status); //set up for $? status checking.
                }
            }
        }
        free_tokens(args);
    }
    free(line);
    return 0;
}