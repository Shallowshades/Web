/**
 * @file client_udp.c
 * @author your name (you@domain.com)
 * @brief UDP广播客户端
 * @version 0.1
 * @date 2022-07-09
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
    //客户端绑定本地端口
    struct sockaddr_in clientAddr;
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(9999);
    clientAddr.sin_addr.s_addr = INADDR_ANY;
    int ret = bind(fd, (struct sockaddr*)&clientAddr, sizeof(clientAddr));
    if(ret == -1){
        perror("bind");
        exit(-1);
    }
    //通信
    char recvBuf[128];
    while (1){
        bzero(recvBuf, sizeof(recvBuf));
        //接收数据 
        int recvLen = recvfrom(fd,recvBuf,sizeof(recvBuf),0,NULL,NULL);
        //打印信息
        printf("server : %s\n", recvBuf); 
    }
    
    close(fd);

    return 0;
}