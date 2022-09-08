/**
 * @file client.c
 * @author your name (you@domain.com)
 * @brief TCP通信客户端
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
#include <sys/stat.h>
#include <fcntl.h>
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
    char* data = "Hello, I'm the client!";
    while(1){
        //给服务器发送数据
        write(fd, data, strlen(data));
        sleep(1);

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
    }

    //关闭文件描述符
    close(fd);

    return 0;
}