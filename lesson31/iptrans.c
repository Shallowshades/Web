/*
    //旧函数  仅适用于IPv4
    #include <arpa/inet.h>
    in_addr_t inet_addr(const char *cp);
    int inet_aton(const char *cp, struct in_addr *inp);
    char *inet_ntoa(struct in_addr in);
    
    //新函数 IPv4/IPv6都适用
    #include <arpa/inet.h>
    // p:点分十进制的IP字符串，n:表示network，网络字节序的整数
    int inet_pton(int af, const char *src, void *dst);
        af:地址族： AF_INET  AF_INET6
        src:需要转换的点分十进制的IP字符串
        dst:转换后的结果保存在这个里面
        返回值：1成功，非1失败

    // 将网络字节序的整数，转换成点分十进制的IP地址字符串
    const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
        af:地址族： AF_INET  AF_INET6
        src: 要转换的ip的整数的地址
        dst: 转换成IP地址字符串保存的地方
        size：第三个参数的大小（数组的大小）
        返回值：返回转换后的数据的地址（字符串），和 dst 是一样的
*/
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){

    char *ip = "192.168.23.128";
    unsigned int num = 0;
    // p:点分十进制的IP字符串，n:表示network，网络字节序的整数
    int ret1 = inet_pton(AF_INET, ip, &num);
    printf("ip  : %s\n", ip);
    printf("num : %ud\n", num);

    // 将网络字节序的整数，转换成点分十进制的IP地址字符串
    // 此处申请栈中内存可以，但是申请堆中内存出错
    // char buf[16]; OK
    // char *buf = (char*)malloc(sizeof(char)*16); NOT OK
    
    printf("sizeof(char) = %lu\n", sizeof(char));
    char buf[16];
    const char *p;
    p = inet_ntop(AF_INET, &num, buf, sizeof(buf));
    printf("p   : %p\nbuf : %p\n", (void*)p, (void*)buf);
    printf("buf : %s\n", buf);
    return 0;
}