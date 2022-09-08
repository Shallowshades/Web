/**
 * @file epoll.c
 * @author your name (you@domain.com)
 * @brief epoll两种模式et 边沿触发
 * @version 0.1
 * @date 2022-07-08
 * 
 * @copyright Copyright (c) 2022
 * 
 */
/*
epoll中有两种触发模式，分别为
1、水平触发
水平触发为Level Triggered，简称LT。
水平触发关心的是缓冲区的状态，当缓冲区可读的时候，就会发出通知，也就是当缓冲区中只要有数据就会发出通知。

2、边缘触发
边缘触发为Edge Triggered，简称ET。
边缘触发关心的是缓冲区状态的变化，当缓冲区状态发生变化的时候才会发出通知，比如缓冲区中来了新的数据。

从上述表述可能不太看得出他们之间的区别，我们设想这样一个场景，当一次read()读取没有读取完缓冲区中的数据时，LT和ET的区别：
1、LT，此时缓冲区中还有数据，会继续发通知

2、ET，此时缓冲区状态并没有发生变化，并没有来新的数据，就不会发通知，在新数据到来之前，之前剩余的数据就无法取出。

所以在ET模式下，当读取数据的时候，一定要循环读取数据，直到缓冲区中的数据全部读取完成，一次性将数据取出。
*/
#include <stdio.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>

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

                //设置cfd属性非阻塞
                int flag = fcntl(cfd, F_GETFL);
                flag |= O_NONBLOCK;
                fcntl(cfd, F_SETFL, flag);

                //获取客户端的信息
                char clientIP[16];
                inet_ntop(AF_INET, &clientAddr.sin_addr.s_addr, clientIP, sizeof(clientIP));
                unsigned short clientPort = ntohs(clientAddr.sin_port);
                printf("client ip is : %s, port is : %u\n", clientIP, clientPort);

                //将新的客户端的文件描述符加入到epoll实例中 EPOLLET设置边沿触发
                epev.events = EPOLLIN | EPOLLET;
                epev.data.fd = cfd;
                epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&epev);

            }else{
                if(epevs[i].events & EPOLLOUT){
                    continue;
                }

                //说明对应的文件描述符发来了数据
                char recvBuf[6] = {0};
                int recvLen = 0;
                while((recvLen = read(epevs[i].data.fd, recvBuf, sizeof(recvBuf) - 1) ) > 0){
                    printf("recv data : %s\n",  recvBuf);
                    write(epevs[i].data.fd, recvBuf, recvLen);
                }

                if(recvLen == 0){
                    printf("client closed...\n");
                    //将断开连接的客户端的文件描述符从epoll实例中清除
                    epoll_ctl(epfd, EPOLL_CTL_DEL, epevs[i].data.fd, NULL);
                    close(epevs[i].data.fd);
                }else if(recvLen == -1){
                    if(errno == EAGAIN) {
                        printf("data over.....\n");
                    }else {
                        perror("read");
                        exit(-1);
                    }
                }
            }
        }
    }

    close(lfd);
    close(epfd);

    return 0;
}