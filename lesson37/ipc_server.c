/**
 * @file ipc_server.c
 * @author your name (you@domain.com)
 * @brief 本地套接字通信服务器(TCP)
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

    //删除文件
    unlink("server.sock");

    //创建用于监听的套接字
    int lfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if(lfd == -1){
        perror("socket");
        exit(-1);
    }

    //绑定本地套接字文件
    struct sockaddr_un serverAddr;
    serverAddr.sun_family = AF_LOCAL;
    strcpy(serverAddr.sun_path, "server.sock");
    int ret = bind(lfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if(ret == -1){
        perror("bind");
        exit(-1);
    }
    //监听
    ret = listen(lfd, 100);
    if(ret == -1){
        perror("listen");
        exit(-1);
    }

    //等待客户端连接
    struct sockaddr_un clientAddr;
    int len = sizeof(clientAddr);
    int cfd = accept(lfd, (struct sockaddr*)&clientAddr, &len);
    if(cfd == -1){
        perror("accept");
        exit(-1);
    }
    printf("client socket filename : %s\n", clientAddr.sun_path);

    //通信
    char recvBuf[128];
    while(1){
        int recvLen = recv(cfd, recvBuf, sizeof(recvBuf), 0);
        if(recvLen == -1){
            perror("recv");
            exit(-1);
        }else if(recvLen == 0){
            printf("client closed...\n");
            break;
        }else if(recvLen > 0){
            printf("client : %s\n", recvBuf);
            send(cfd, recvBuf,strlen(recvBuf), 0);
        }
    }
    close(lfd);
    close(cfd);
    return 0;
}