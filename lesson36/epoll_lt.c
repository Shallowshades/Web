/**
 * @file epoll.c
 * @author your name (you@domain.com)
 * @brief epoll两种工作模式 lt 水平触发 缺省
 * @version 0.1
 * @date 2022-07-08
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

int main(){

    //创建一个用于监听的socket
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lfd == -1){
        perror("socket");
        exit(-1);
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(9999);

    //绑定端口
    int ret = bind(lfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if(ret == -1){
        perror("bind");
        exit(-1);
    }

    //监听
    ret = listen(lfd, 8);
    if(ret == -1){
        perror("listen");
        exit(-1);
    }

    //创建一个epoll实例 传参被忽略，但是要大于零 
    int epfd = epoll_create(100);

    //epoll_event结构体
    struct epoll_event epev;
    epev.events = EPOLLIN;
    epev.data.fd = lfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &epev);

    struct epoll_event epevs[1024];

    while(1){
        //调用epoll_wait函数，让内核帮忙检测哪些文件描述符有数据 -1阻塞
        ret = epoll_wait(epfd, epevs, 1024, -1);
        if(ret == -1){
            perror("epoll_wait");
            exit(-1);
        }

        //大于零说明有几个文件描述符对应的缓冲区数据发生了变化
        printf("ret = %d\n", ret);

        for(int i = 0; i < ret; ++i){
            if(epevs[i].data.fd == lfd){
                //表示监听的文件描述符有数据到达，有客户端连接
                struct sockaddr_in clientAddr;
                int len = sizeof(clientAddr);
                int cfd = accept(lfd, (struct sockaddr*)&clientAddr, &len);

                //获取客户端的信息
                char clientIP[16];
                inet_ntop(AF_INET, &clientAddr.sin_addr.s_addr, clientIP, sizeof(clientIP));
                unsigned short clientPort = ntohs(clientAddr.sin_port);
                printf("client ip is : %s, port is : %u\n", clientIP, clientPort);

                //将新的客户端的文件描述符加入到epoll实例中
                epev.events = EPOLLIN;
                epev.data.fd = cfd;
                epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&epev);

            }else{
                //说明对应的文件描述符发来了数据
                char recvBuf[5] = {0};
                int recvLen = read(epevs[i].data.fd, recvBuf, sizeof(recvBuf));
                if(recvLen == -1){
                    perror("read");
                    exit(-1);
                }else if(recvLen == 0){
                    printf("client closed...\n");
                    //将断开连接的客户端的文件描述符从epoll实例中清除
                    epoll_ctl(epfd, EPOLL_CTL_DEL, epevs[i].data.fd, NULL);
                    close(epevs[i].data.fd);
                }else if(recvLen > 0){
                    printf("read recvBuf = %s\n", recvBuf);
                    write(epevs[i].data.fd, recvBuf, strlen(recvBuf)+1);
                }
            }
        }
    }

    close(lfd);
    close(epfd);

    return 0;
}