/**
 * @file chatA1.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-06-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void parent_process();
void son_process();
void send();
void receive();

int main(){

    int pid = fork();
    if(pid > 0){//父进程
        parent_process();
        wait(NULL);
        printf("子进程已回收\n");
    }else{//子进程
        son_process();
    }
    return 0;
}

void parent_process(){
    int ret = access("fifo1",F_OK);
    if(ret == -1){
        printf("fifo1管道文件不存在，创建管道文件\n");
        ret = mkfifo("fifo1",0664);
        if(ret == -1){
            perror("mkfifo");
            exit(0);
        }else{
            printf("fifo1管道文件创建成功\n");
        }
    }else{
        printf("fifo1管道文件已存在,开始进程\n");
    }
    send();
}
void son_process(){
    int ret = access("fifo2", F_OK);
    if(ret == -1){
        printf("fifo2管道文件不存在，创建管道文件\n");
        ret = mkfifo("fifo2",0664);
        if(ret == -1){
            perror("mkfifo");
            exit(0);
        }else{
            printf("fifo2管道文件创建成功\n");
        }
    }else{
        printf("fifo2管道文件已存在,开始进程\n");
    }
    receive();
}
void send(){

    //只写方式打开fifo1
    int fdw = open("fifo1", O_WRONLY);
    if(fdw == -1){
        perror("open fifo1");
        exit(0);
    }
    printf("打开fifo1成功，等待写入数据\n");

    char buf[128];
    //循环读取
    while(1){
        //写数据
        memset(buf,0,sizeof(buf));
        fgets(buf,128,stdin);
        int ret = write(fdw, buf, strlen(buf));
        if(ret == -1){
            perror("write");
            exit(0);
        }
    }
    close(fdw);
}
void receive(){
    //只读方式打开fifo2
    int fdr = open("fifo2", O_RDONLY);
    if(fdr == -1){
        perror("open fifo2");
        exit(0);
    }
    printf("打开fifo2成功，等待读取\n");
    char buf[128];
    //循环读取
    while(1){
        //读数据
        memset(buf,0,sizeof(buf));
        int ret = read(fdr, buf, sizeof(buf));
        if(ret <= 0){
            perror("read");
            exit(0);
        }
        printf("B: %s\n",buf);
    }
    close(fdr);
}