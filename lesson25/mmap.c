#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[]){

    if(argc < 2){
        printf("参数不足，请输入用户名称\n");
        exit(0);
    }

    //1. 打开一个文件
    int fd = open("test.txt",O_RDWR);
    //int size = lseek(fd,0,SEEK_END);    //获取文件大小
    int size = 4096;

    //2. 创建内存映射区
    void *ptr = mmap(NULL,size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    if(ptr == MAP_FAILED){
        perror("mmap");
        exit(0);
    }

    //3. 创建子进程
    pid_t pid = fork();
    memset(ptr,0,sizeof(ptr));
    if(pid > 0){//父进程
        char buf[1024] = {0};
        while(1){
            if(strlen(ptr) == 0){
                strcpy(buf,argv[1]);
                strcat(buf,": \0");
                fgets(buf + strlen(buf), sizeof(buf),stdin);
                strcpy(ptr,buf);
                memset(buf,0,sizeof(buf));
            }
        }
        wait(NULL);
    }else if(pid == 0){
        char buf[1024] = {0};
        while(1){
            if(strlen(ptr) > 0 && strncmp(argv[1],ptr,strlen(argv[1])) != 0){
                strcpy(buf,(char*)ptr);
                printf("%s\n", buf);
                memset(buf,0,sizeof(buf));
                memset(ptr,0,sizeof(ptr));
            }
        }
    }else{
        printf("something error\n");
    }

    munmap(ptr,size);
    return 0;
}