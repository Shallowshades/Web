/**
 * @file client.c
 * @author your name (you@domain.com)
 * @brief 并发服务器通信客户端
 * @version 0.1
 * @date 2022-07-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(){

    //1. 创建套接字
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1){
        perror("socket");
        exit(-1);
    }

    //2.连接服务器
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "192.168.29.135", &serverAddr.sin_addr.s_addr);
    serverAddr.sin_port = htons(9999);
    int ret = connect(fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if(ret == -1){
        perror("connect");
        exit(-1);
    }

    //3.通信
    char recvBuf[1024];
    char sendBuf[1024];
    int num = 1;
    while(1){
        //memset(recvBuf, 0, sizeof(recvBuf));
        //memset(sendBuf, 0, sizeof(sendBuf));

        //给服务器发送数据
        sprintf(sendBuf, "num = %d\n", num++);
        write(fd, sendBuf, strlen(sendBuf) + 1);

        //接收服务器发来的数据
        int len = read(fd, recvBuf, sizeof(recvBuf));
        if(len == -1){
            perror("read");
            exit(-1);
        }else if(len > 0){
            printf("recv server data : %s\n", recvBuf);
        }else if(len == 0){
            //表示服务器断开连接
            printf("server closed...\n");
            break;
        }

        //这个睡眠的位置？？？
        sleep(1);
    }

    //关闭文件描述符
    close(fd);

    return 0;
}