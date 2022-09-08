#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/signal.h>

int main(){

    pid_t pid = fork();

    if(pid == 0){
        for(int i=0;i<5;++i){
            printf("I'm child process\n");
            sleep(1);
        }
    }else if(pid > 0){
        printf("I'm parent process\n");
        sleep(3);
        printf("kill child process now\n");
        if(kill(pid,SIGINT) == 0){
            printf("kill successfully!\n");
        }else{
            printf("kill unsuccessfully!\n");
        }

    }

    return 0;
}