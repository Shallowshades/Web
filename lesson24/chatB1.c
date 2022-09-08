/**
 * @file chatB1.c
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

int main(){

    int pid = fork();
    if(pid > 0){//父进程
        int ret = access("fifo2",F_OK);
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

        //只写方式打开fifo2
        int fdw = open("fifo2", O_WRONLY);
        if(fdw == -1){
            perror("open fifo2");
            exit(0);
        }
        printf("打开fifo2成功，等待写入数据\n");

        char buf[128];
        //循环读取
        while(1){
            //写数据
            memset(buf,0,sizeof(buf));
            fgets(buf,128,stdin);
            ret = write(fdw, buf, strlen(buf));
            if(ret == -1){
                perror("write");
                exit(0);
            }
        }
        close(fdw);

        //等待子进程结束回收资源
        wait(NULL);
        printf("子进程已回收\n");
    }else{//子进程
        int ret = access("fifo1", F_OK);
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
        //只读方式打开fifo1
        int fdr = open("fifo1", O_RDONLY);
        if(fdr == -1){
            perror("open fifo1");
            exit(0);
        }
        printf("打开fifo1成功，等待读取\n");
        char buf[128];
        //循环读取
        while(1){
            //读数据
            memset(buf,0,sizeof(buf));
            ret = read(fdr, buf, sizeof(buf));
            if(ret <= 0){
                perror("read");
                exit(0);
            }
            printf("A: %s\n",buf);
        }
        close(fdr);
    }

    return 0;
}