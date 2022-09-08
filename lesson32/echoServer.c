/**
 * @file echoServer.c
 * @author your name (you@domain.com)
 * @brief 回射服务器， 客户端发送什么， 服务器返回什么
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

    //1. 创建一个用于监听的套接字
    //SOCK_STREAM 流式传输 0 默认TCP
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd == -1) {
        perror("socket");
        exit(-1);
    }

    //2. 将这个监听文件描述符和本地的IP和端口绑定（IP和端口就是服务器的地址信息）
    //- 客户端连接服务器的时候使用的就是这个IP和端口
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    //inet_pton(AF_INET, "192.168.29.134", saddr.sin_addr.s_addr);
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(8080);
    int ret = bind(sfd,(struct sockaddr*)&saddr, sizeof(saddr));
    if(ret == -1) {
        perror("bind");
        exit(-1);
    }
     
    //3. 设置监听，监听的fd开始工作
    ret = listen(sfd, 8);
    if(ret == -1) {
        perror("listen");
        exit(-1);
    }

    //4. 阻塞等待，当有客户端发起连接，解除阻塞，接受客户端的连接，
    //会得到一个和客户端通信的套接字（fd）
    struct sockaddr_in clientaddr;
    socklen_t len = sizeof(clientaddr);
    int cfd = accept(sfd, (struct sockaddr*)&clientaddr, &len);
    if(cfd == -1){
        perror("accept");
        exit(-1);
    }

    //输出客户端的信息
    char clientIP[16];
    inet_ntop(AF_INET, &clientaddr.sin_addr.s_addr, clientIP, sizeof(clientIP));
    unsigned short clientPort = ntohs(clientaddr.sin_port);
    printf("client information :\nip : %s\nport : %u\n", clientIP, clientPort);

    //5. 通信
    char recvBuf[1024];
    while(1){
        //- 接收数据
        bzero(recvBuf, sizeof(recvBuf));
        //memset(recvBuf, 0, sizeof(recvBuf));
        int recvLen = read(cfd, recvBuf, sizeof(recvBuf));
        if(recvLen == -1){
            perror("read");
            exit(-1);
        }else if(recvLen > 0){
            printf("recv client data : %s\n", recvBuf);
        }else if(recvLen == 0){
            //表示客户端断开连接
            printf("client closed...\n");
            break;
        }

        //- 发送数据
        write(cfd, recvBuf, strlen(recvBuf));
    }

    //关闭文件描述符
    close(sfd);
    close(cfd);

    //6. 通信结束，断开连接


    return 0;
}