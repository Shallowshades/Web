/**
 * @file stat.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-06-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/*
       #include <sys/types.h>
       #include <sys/stat.h>
       #include <unistd.h>

       int stat(const char *pathname, struct stat *statbuf);
        作用：获取一个文件的相关信息
        参数：
            -pathname:  操作文件的路径
            -statbuf:   结构体变量，传出参数，用于保存获取到的文件信息
        返回值：
            -0成功， -1失败并且设置errno

       int fstat(int fd, struct stat *statbuf);
       int lstat(const char *pathname, struct stat *statbuf);
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

int main(){

    struct stat statbuf;
    int ret = stat("a.txt",&statbuf);
    if(ret == -1){
        perror("stat");
        return -1;
    }

    printf("size: %ld\n", statbuf.st_size);
    

    return 0;
}
