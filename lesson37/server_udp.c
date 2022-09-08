/**
 * @file server_udp.c
 * @author your name (you@domain.com)
 * @brief UDP服务器
 * @version 0.1
 * @date 2022-07-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(){

    //创建一个通信的socket
    int fd = socket(PF_INET, SOCK_DGRAM, 0);
    if(fd == -1){
        perror("socket");
        exit(-1);
    }
    //服务器本地地址
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9999);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    //绑定
    int ret = bind(fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if(ret == -1){
        perror("bind");
        exit(-1);
    }
    //通信
    char recvBuf[128];
    char sendBuf[128];
    char ipBuf[16];
    struct sockaddr_in clientAddr;
    int len = sizeof(clientAddr);
    while (1){
        //接收数据
        int recvLen = recvfrom(fd,recvBuf,sizeof(recvBuf),0,(struct sockaddr*)&clientAddr,&len);
        //打印客户端信息
        printf("client IP : %s, Port : %u\n",
            inet_ntop(AF_INET, &clientAddr.sin_addr.s_addr,ipBuf,sizeof(ipBuf)),
            ntohs(clientAddr.sin_port)
        );
        //打印客户端显示信息
        printf("client : %s\n", recvBuf);
        //发送信息
        ret = sendto(fd, recvBuf, strlen(recvBuf) + 1, 0, (struct sockaddr*)&clientAddr, sizeof(clientAddr));
        if(ret == -1){
            perror("sendto");
            exit(-1);
        }
    }

    close(fd);

    return 0;
}