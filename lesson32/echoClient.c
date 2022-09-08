/**
 * @file echoClient.c
 * @author your name (you@domain.com)
 * @brief 向回射服务器发送数据，并接收回射服务器返回的数据
 * @version 0.1
 * @date 2022-07-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(){

    //1.创建套接字
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1){
        perror("socket");
        exit(-1);
    }

    //2.连接服务器
    struct sockaddr_in serverSocket;
    serverSocket.sin_family = AF_INET;
    int ret = inet_pton(AF_INET, "192.168.29.134", &serverSocket.sin_addr.s_addr);
    if(ret != 1){
        perror("inet_pton");
        exit(-1);
    }
    serverSocket.sin_port = htons(9999);
    //通信到CentOS服务器
    // int ret = inet_pton(AF_INET, "121.4.105.146", &serverSocket.sin_addr.s_addr);
    // if(ret != 1){
    //     perror("inet_pton");
    //     exit(-1);
    // }
    // serverSocket.sin_port = htons(8080);
    ret = connect(fd, (struct sockaddr*)&serverSocket, sizeof(serverSocket));
    if(ret == -1){
        perror("connect");
        exit(-1);
    }

    //3.通信
    //fgets(buf, sizeof(buf),stdin);
    char recvBuf[1024];
    char sendBuf[1024];
    while(1){
        memset(recvBuf, 0, sizeof(recvBuf));
        memset(recvBuf, 0, sizeof(sendBuf));
        //给服务器发送数据
        fgets(sendBuf, sizeof(sendBuf), stdin);
        write(fd, sendBuf, strlen(sendBuf));

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