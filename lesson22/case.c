/*
    实现 ps aux | grep xxx 父子进程间通信
    
    子进程： ps aux, 子进程结束后，将数据发送给父进程
    父进程：获取到数据，过滤
    pipe()
    execlp()
    子进程将标准输出 stdout_fileno 重定向到管道的写端。  dup2
*/

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>

int main(){
    int pipefd[2];
    int ret = pipe(pipefd);
    if(ret == -1) {
        perror("pipe");
        exit(0);
    }
    pid_t pid = fork();
    if(pid > 0){//父进程
        close(pipefd[1]);
        char buf[1024] = {0};
        int len = -1;
        while((len = read(pipefd[0],buf,sizeof(buf)-1)) > 0){
            printf("%s",buf);
            memset(buf,0,sizeof(buf));
        }
        wait(NULL);
    }else if(pid == 0){//子进程
        close(pipefd[0]);
        //文件描述符的重定向， STDOUT_FILENO -> pipefd[1]
        dup2(pipefd[1], STDOUT_FILENO); 
        execlp("ps","ps","aux",NULL);
    }else{
        perror("fork");
        exit(0);
    }
    return 0;
}