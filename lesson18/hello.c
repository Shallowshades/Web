#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {

    printf("begin\n");

    if(fork() > 0) {

        printf("我是父进程：pid = %d, ppid = %d, pgid = %d\n", getpid(), getppid(), getpgid(getpid()));

        int i;
        for(i = 0; i < 10; i++) {
            printf("i = %d\n", i);
            sleep(1);
        }

    } else {

        printf("我是子进程：pid = %d, ppid = %d pgid = %d\n", getpid(), getppid(),getpgid(getpid()));
        
        int j;
        for(j = 0; j < 10; j++) {
            printf("j = %d\n", j);
            sleep(1);
        }

    }

    return 0;
}
/*
我是父进程：pid = 21464, ppid = 20303, pgid = 21464
我是子进程：pid = 21465, ppid = 21464 pgid = 21464

USER    PID   %CPU %MEM VSZ    RSS  TTY    STAT  START  TIME COMMAND
shallow 20303 0.0  0.1  24592  5344 pts/5  Ss    15:58  0:00 /bin/bash
*/