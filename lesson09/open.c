/**
 * @file open.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-06-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/*
       #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>

       
       //C中没有重载，用可变参数实现

       //打开一个已经存在的文件
       int open(const char *pathname, int flags);
        参数：
            -pathname 要打开的文件路径
            -flags 对文件的操作权限，还有其他设置
            O_RDONLY, O_WRONLY, O_RDWR 这三个设置互斥
        返回值：
            返回一个新的文件描述符，如果调用失败，返回-1
        
        errno:
            属于linux系统函数库，库里面的一个全局变量，记录的是最近的一个错误号
        
       #include <stdio.h>
       void perror(const char *s);作用：打印errno对应的错误描述
        s参数：用户描述，比如hello,最终输出的内容是  hello:xxx(实际的错误描述)

       //创建一个新的文件
       int open(const char *pathname, int flags, mode_t mode);

*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(){

    int fd = open("a.txt", O_RDONLY);
    if(fd == -1){
        perror("open");
    }else{
        printf("open successful\n");
    }
    close(fd);
    return 0;
}