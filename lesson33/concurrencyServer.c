/**
 * @file concurrencyServer.c
 * @author your name (you@domain.com)
 * @brief 多进程实现并发服务器
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
#include <signal.h>
#include <wait.h>
#include <errno.h>

//信号捕捉的回调函数：回收子进程资源
void recyleChild(int arg){
    while(1){
        int ret = waitpid(-1, NULL, WNOHANG);
        if(ret == -1){
            //所有子进程都被回收了
            break;
        }else if(ret == 0){
            //还有子进程活着
            break;
        }else if(ret > 0){
            //ret为被回收子进程的pid
            printf("child process pid : %d had recycled\n", ret);
        }
    }

}

int main(){

    //
    struct sigaction act;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    act.sa_handler = recyleChild;

    //注册信号捕捉
    sigaction(SIGCHLD, &act, NULL);

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

    //循环等待连接
    while(1){
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int cfd = accept(lfd, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if(cfd == -1){
            if(errno == EINTR){
                continue; 
            }
            perror("accept");
            exit(-1);
        }

        pid_t pid = fork();
        if(pid == 0){
            //子进程
            //获取客户端的信息
            char clientIP[16];
            inet_ntop(AF_INET, &clientAddr.sin_addr.s_addr, clientIP, sizeof(clientIP));
            unsigned short clientPort = ntohs(clientAddr.sin_port);
            printf("client ip is : %s, port is : %u\n", clientIP, clientPort);

            //通信 
            char recvBuf[1024];
            while(1){
                //memset(recvBuf, 0, sizeof(recvBuf));

                //接收客户端的信息
                int len = read(cfd, recvBuf, sizeof(recvBuf));
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
                write(cfd, recvBuf, strlen(recvBuf) + 1);
            }
            //关闭文件描述符
            close(cfd);
            //退出当前子进程
            exit(0);
        }
    }

    close(lfd);

    return 0;
}