/**
 * @file client_udp.c
 * @author your name (you@domain.com)
 * @brief UDP客户端
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
    //服务器地址
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9999);
    inet_pton(AF_INET, "192.168.29.135", &serverAddr.sin_addr.s_addr);
    //通信
    char recvBuf[128];
    char sendBuf[128];
    int num = 0;
    while (1){
        bzero(sendBuf, sizeof(sendBuf));
        bzero(recvBuf, sizeof(recvBuf));
        //发送信息
        sprintf(sendBuf, "hello, I'm client %d\n", num++);
        int ret = sendto(fd, sendBuf, strlen(sendBuf) + 1, 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        if(ret == -1){
            perror("sendto");
            exit(-1);
        }
        //接收数据 
        int recvLen = recvfrom(fd,recvBuf,sizeof(recvBuf),0,NULL,NULL);
        //打印信息
        printf("server : %s\n", recvBuf);
        sleep(1);
    }
    
    close(fd);

    return 0;
}