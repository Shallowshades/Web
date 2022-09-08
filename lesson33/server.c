/**
 * @file server.c
 * @author your name (you@domain.com)
 * @brief 多线程实现并发服务器
 * @version 0.1
 * @date 2022-07-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

struct sockInfo{
    int fd;//通信的文件描述符
    struct sockaddr_in clientAddr;//客户端的信息
    pthread_t tid;//线程号
};

struct sockInfo sockInfos[1024];

void* working(void* arg){
    //子线程和客户端进行通信 cfd clientAddr tid等信息
    //获取客户端的信息
    struct sockInfo* pinfo = (struct sockInfo*)arg;
    char clientIP[16];
    inet_ntop(AF_INET, &pinfo->clientAddr.sin_addr.s_addr, clientIP, sizeof(clientIP));
    unsigned short clientPort = ntohs(pinfo->clientAddr.sin_port);
    printf("client ip is : %s, port is : %u\n", clientIP, clientPort);

    //通信 
    char recvBuf[1024];
    while(1){
        //memset(recvBuf, 0, sizeof(recvBuf));

        //接收客户端的信息
        int len = read(pinfo->fd, recvBuf, sizeof(recvBuf));
        if(len == -1){
            perror("read");
            exit(-1);
        }else if(len > 0){
            printf("recv client data : %s\n", recvBuf);
        }else if(len == 0){
            printf("clinet closed...\n");
            break;
        }

        //回射
        write(pinfo->fd, recvBuf, strlen(recvBuf) + 1);
    }
    //关闭文件描述符
    close(pinfo->fd);
    pinfo->fd = -1;
    //退出当前子线程
    pthread_exit(NULL);
}

int main(){

    //1.创建一个用于监听的套接字
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lfd == -1){
        perror("socket");
        exit(-1);
    }

    //2.绑定端口
    struct sockaddr_in localAddr;
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = INADDR_ANY;
    localAddr.sin_port = htons(9999);

    int ret = bind(lfd, (struct sockaddr*)&localAddr, sizeof(localAddr));
    if(ret == -1){
        perror("bind");
        exit(-1);
    }

    //3.监听
    ret = listen(lfd, 128);
    if(ret == -1){
        perror("listen");
        exit(-1);
    }

    //初始化客户端套接字信息数组
    int max = sizeof(sockInfos)/sizeof(struct sockInfo);
    for(int i=0;i<max;++i){
        bzero(&sockInfos[i], sizeof(struct sockInfo));
        sockInfos[i].fd = -1;
        sockInfos[i].tid = -1;
    }

    //循环等待连接
    while(1){
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int cfd = accept(lfd, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if(cfd == -1){
            perror("accept");
            exit(-1);
        }

        //创建子线程，并且提供子线程所需的数据
        //struct sockInfo* pinfo = (struct sockInfo*)malloc(sizeof(struct sockInfo));
        struct sockInfo* pinfo;
        for(int i = 0; i < max; ++i){
            if(sockInfos[i].fd == -1){
                pinfo = &sockInfos[i];
                break;
            }

            //当前子线程已达上限，睡眠等待
            if(i == max-1){
                sleep(1);
                i = 0;
            }
        }
        pinfo->fd = cfd;
        memcpy(&pinfo->clientAddr, &clientAddr, sizeof(clientAddr));

        //创建子线程
        pthread_create(&pinfo->tid, NULL, working, pinfo);

        //线程分离，非阻塞
        pthread_detach(pinfo->tid);
    }

    close(lfd);

    return 0;
}