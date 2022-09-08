/**
 * @file select.c
 * @author your name (you@domain.com)
 * @brief select实现IO多路复用
 * @version 0.1
 * @date 2022-07-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
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

    //创建一个fd_set集合，存放需要监测的文件描述符
    //printf("%ld\n", sizeof(long int)); 8
    fd_set rdset, tmp;
    FD_ZERO(&rdset);
    FD_SET(lfd, &rdset);
    int maxfd = lfd;

    while(1){
        tmp = rdset;
        //调用select函数，让内核帮忙检测哪些文件描述符有数据
        ret = select(maxfd+1, &tmp, NULL, NULL, NULL);
        if(ret == -1){
            perror("select");
            exit(-1);
        }else if(ret == 0){
            continue;
        }else if(ret > 0){
            //说明有文件描述符对应的缓冲区数据发生了变化
            if(FD_ISSET(lfd, &tmp)){
                struct sockaddr_in clientAddr;
                int len = sizeof(clientAddr);
                int cfd = accept(lfd, (struct sockaddr*)&clientAddr, &len);

                //将新的文件描述符加入到集合中
                FD_SET(cfd, &rdset);

                //更新最大的文件描述符
                maxfd = maxfd > cfd ? maxfd : cfd;
            }

            //循环遍历所有的文件描述符，读取有数据的缓冲区
            for(int i=lfd+1;i<maxfd;++i){
                if(FD_ISSET(i,&tmp)){
                    //说明对应的文件描述符发来了数据
                    char recvBuf[1024] = {0};
                    int recvLen = read(i, recvBuf, sizeof(recvBuf));
                    if(recvLen == -1){
                        perror("read");
                        exit(0);
                    }else if(recvLen == 0){
                        printf("client closed...\n");
                        close(i);
                        FD_CLR(i, &rdset);
                    }else if(recvLen > 0){
                        printf("read recvBuf = %s\n", recvBuf);
                        write(i, recvBuf, strlen(recvBuf)+1);
                    }
                }
            }

        }

    }

    close(lfd);

    return 0;
}