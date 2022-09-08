/**
 * @file readv.c
 * @author your name (you@domain.com)
 * @brief 分散读和分散写
 * @version 0.1
 * @date 2022-07-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <unistd.h>
#include <stdio.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

/*
    #include <sys/uio.h>
    ssize_t readv(int filedes, const struct iovec *iov, int iovcnt);
        //从连续的一块，读到多块不连续的地方，总是先填满前面的，再顺序往后填写

    ssize_t writev(int filedes, const struct iovec *iov, int iovcnt);
        //从多块不连续的地方，读到连续的一块
    
    两个函数的第二个参数都是一个iovec结构体数组的指针：
    struct iovec {
        void *iov_base;  starting address of buffer 
        size_t iov_len;  size of buffer 
    };
*/

int main(){

    struct iovec iv[3];
    char ptr1[10] = {'\0'}, ptr2[10] = {'\0'}, ptr3[10] = {'\0'};
    int rfd = open("a.txt", O_RDONLY);
    iv[0].iov_base = ptr1;
    iv[0].iov_len = 9;
    iv[1].iov_base = ptr2;
    iv[1].iov_len = 9;
    iv[2].iov_base = ptr3;
    iv[2].iov_len = 9;
    readv(rfd, iv, 3);
    printf("ptr1 = %s\n", ptr1);
    printf("ptr2 = %s\n", ptr2);
    printf("ptr3 = %s\n", ptr3);

    int wfd = open("b.txt", O_RDWR | O_CREAT | O_TRUNC,  0664);
    writev(wfd, iv, 3);

    return 0;
}