#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h> 
#include <unistd.h>

int main(){

    int fd = open("fifo", O_RDONLY);
    if(fd == -1){
        perror("open");
        exit(0);
    }

    //读数据
    while(1){
        char buf[1024] = {0};
        int len = read(fd,buf,sizeof(buf));
        if(len == 0){
            printf("写端断开连接\n");
            break;
        }
        printf("recv buf : %s\n", buf);
    }

    return 0;
}