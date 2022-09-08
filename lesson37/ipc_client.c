/**
 * @file ipc_client.c
 * @author your name (you@domain.com)
 * @brief 本地套接字通信客户端(TCP)
 * @version 0.1
 * @date 2022-07-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <arpa/inet.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){

    unlink("client.sock");

    //创建用于通信的套接字
    int cfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if(cfd == -1){
        perror("socket");
        exit(-1);
    }

    //绑定本地套接字文件
    struct sockaddr_un clientAddr;
    clientAddr.sun_family = AF_LOCAL;
    strcpy(clientAddr.sun_path, "client.sock");
    int ret = bind(cfd, (struct sockaddr*)&clientAddr, sizeof(clientAddr));
    if(ret == -1){
        perror("bind");
        exit(-1);
    }

    //连接服务器
    struct sockaddr_un serverAddr;
    serverAddr.sun_family = AF_LOCAL;
    strcpy(serverAddr.sun_path, "server.sock"); 
    ret = connect(cfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if(cfd == -1){
        perror("connect");
        exit(-1);
    }

    //通信
    int num = 0;
    char sendBuf[128];
    char recvBuf[128];
    while(1){
        bzero(sendBuf, sizeof(sendBuf));
        bzero(recvBuf, sizeof(recvBuf));
        //发送数据
        sprintf(sendBuf, "hello, I'm client %d\n", num++);
        send(cfd, sendBuf, strlen(sendBuf)+1, 0);
        printf("client : %s\n", sendBuf);

        //接收数据
        int recvLen = recv(cfd, recvBuf, sizeof(recvBuf), 0);
        if(recvLen == -1){
            perror("recv");
            exit(-1);
        }else if(recvLen == 0){
            printf("server closed...\n");
            break;
        }else if(recvLen > 0){
            printf("server : %s\n", recvBuf);
        }

        sleep(1);
    }
    close(cfd);
    return 0;
}