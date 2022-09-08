/**
 * @file broadcast_server.c
 * @author your name (you@domain.com)
 * @brief UDP广播服务器
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
    //设置广播属性
    int op = 1;
    setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &op, sizeof(op));

    //创建一个广播的地址
    struct sockaddr_in clientAddr;
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(9999);
    inet_pton(AF_INET, "192.168.29.255", &clientAddr.sin_addr.s_addr);
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
        printf("boardcast data : %s\n", sendBuf);
        sleep(1);
    }

    close(fd);

    return 0;
}