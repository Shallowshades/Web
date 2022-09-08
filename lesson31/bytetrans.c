/**
 * @file bytetrans.c
 * @author your name (you@domain.com)
 * @brief 字节序转换
 * @version 0.1
 * @date 2022-07-05
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include <arpa/inet.h>

/**
 * @brief 
 * 
 * @return int 
 */

/*
h   ——host 主机，主机字节序(主机通常为小端存储)
to  ——转换成什么
n   ——network 网络字节序(网络字节序规定大端存储)
*/
int main(){

    //htons 转换端口 主机到网络
    unsigned short a = 0x0102;
    unsigned short b = htons(a);
    printf("a : %x\n", a);
    printf("b : %x\n", b);
    
    //htonl 转换IP 主机到网络
    unsigned char buf[4] = {192,168,1,100};
    unsigned int num = *(unsigned int*)buf;
    unsigned int sum = htonl(num);
    unsigned char *p = (unsigned char*)&sum;
    printf("buf : %d.%d.%d.%d\n", *buf, *(buf+1),*(buf+2),*(buf+3));
    printf("p : %d.%d.%d.%d\n", *p, *(p+1),*(p+2),*(p+3));

    printf("------------------------\n");

    //ntohs 网络到主机
    unsigned char buf1[4] = {1,1,168,192};
    unsigned int num1 = *(unsigned int*)buf1;
    unsigned int sum1 = ntohl(num1);
    unsigned char *p1 = (unsigned char*)&sum1;
    printf("buf1 : %d.%d.%d.%d\n", *buf1, *(buf1+1),*(buf1+2),*(buf1+3));
    printf("p1 : %d %d %d %d\n", *p1, *(p1+1), *(p1+2), *(p1+3));

    //ntohl 网络到主机
    unsigned short a1 = 0x0201;
    unsigned short b1 = htons(a1);
    printf("a1 : %x\n", a1);
    printf("b1 : %x\n", b1);

    return 0;
}