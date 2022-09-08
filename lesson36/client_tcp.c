/**
 * @file client_tcp.c
 * @author your name (you@domain.com)
 * @brief TCP客户端程序
 * @version 0.1
 * @date 2022-07-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {

    // 创建socket
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    if(fd == -1) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in seraddr;
    inet_pton(AF_INET, "192.168.29.135", &seraddr.sin_addr.s_addr);
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(9999);

    // 连接服务器
    int ret = connect(fd, (struct sockaddr *)&seraddr, sizeof(seraddr));

    if(ret == -1){
        perror("connect");
        return -1;
    }

    int num = 0;
    char sendBuf[1024] = {0};
    char recvBuf[1024] = {0};
    while(1) {
        memset(sendBuf, 0, sizeof(sendBuf));
        memset(recvBuf, 0, sizeof(recvBuf));

        //sprintf(sendBuf, "send data %d", num++);
        fgets(sendBuf, sizeof(sendBuf), stdin);
        write(fd, sendBuf, strlen(sendBuf) + 1);

        // 接收
        int len = read(fd, recvBuf, sizeof(recvBuf));
        if(len == -1) {
            perror("read");
            return -1;
        }else if(len > 0) {
            printf("read buf = %s\n", recvBuf);
        } else {
            printf("服务器已经断开连接...\n");
            break;
        }
        usleep(1000);
        //sleep(1);
    }

    close(fd);

    return 0;
}
