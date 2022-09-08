/**
 * @file broadcast_server.c
 * @author your name (you@domain.com)
 * @brief UDP组播(多播)服务器
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
    //设置多播属性，设置外出接口
    struct in_addr multiAddr;
    inet_pton(AF_INET, "239.0.0.10", &multiAddr.s_addr);
    setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, &multiAddr, sizeof(multiAddr));

    //创建一个多播的地址
    struct sockaddr_in clientAddr;
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(9999);
    inet_pton(AF_INET, "239.0.0.10", &clientAddr.sin_addr.s_addr);
    
    //通信
    char sendBuf[128];
    int num = 1;
    while (1){
        //发送信息
        sprintf(sendBuf, "hello, client...%d\n", num++);
        int ret = sendto(fd, sendBuf, strlen(sendBuf) + 1, 0, (struct sockaddr*)&clientAddr, sizeof(clientAddr));
        if(ret == -1){
            perror("sendto");
            exit(-1);
        }
        printf("multicast data : %s\n", sendBuf);
        sleep(1);
    }

    close(fd);

    return 0;
}