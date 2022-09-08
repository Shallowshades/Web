/**
 * @file poll.c
 * @author your name (you@domain.com)
 * @brief poll实现IO多路复用
 * @version 0.1
 * @date 2022-07-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include <unistd.h>
#include <poll.h>
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

    //创建一个pollfd文件描述符数组
    struct pollfd fds[1024];
    for(int i=0;i<1024;++i){
        fds[i].fd = -1;
        fds[i].events = POLLIN;
    }
    fds[0].fd = lfd;

    //最大的文件描述符的脚标
    int nfds = 0;

    while(1){
        
        //调用poll函数，让内核帮忙检测哪些文件描述符有数据
        ret = poll(fds, nfds + 1, -1);//0nonblock -1block >0blocktimeout
        if(ret == -1){
            perror("poll");
            exit(-1);
        }else if(ret == 0){
            continue;
        }else if(ret > 0){
            //说明有文件描述符对应的缓冲区数据发生了变化
            if(fds[0].revents & POLLIN){
                struct sockaddr_in clientAddr;
                int len = sizeof(clientAddr);
                int cfd = accept(lfd, (struct sockaddr*)&clientAddr, &len);

                //将新的文件描述符加入到集合中
                for(int i=1;i<1024;++i){
                    if(fds[i].fd == -1){
                        fds[i].fd = cfd;
                        fds[i].events = POLLIN;

                        //更新最大的文件描述符
                        nfds = nfds > i ? nfds : i;
                        break;
                    }
                }
            }

            //循环遍历所有的文件描述符，读取有数据的缓冲区
            for(int i=1;i<=nfds;++i){
                if(fds[i].revents & POLLIN){
                    //说明对应的文件描述符发来了数据
                    char recvBuf[1024] = {0};
                    int recvLen = read(fds[i].fd, recvBuf, sizeof(recvBuf));
                    if(recvLen == -1){
                        perror("read");
                        exit(0);
                    }else if(recvLen == 0){
                        printf("client closed...\n");
                        close(fds[i].fd);
                        fds[i].fd = -1;
                    }else if(recvLen > 0){
                        printf("read recvBuf = %s\n", recvBuf);
                        write(fds[i].fd, recvBuf, strlen(recvBuf)+1);
                    }
                }
            }

        }

    }

    close(lfd);

    return 0;
}