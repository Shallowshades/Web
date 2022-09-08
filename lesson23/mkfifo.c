/*
    创建fifo文件
    1.通过命令： mkfifo 名字
    2.通过函数：int mkfifo(const char *pathname, mode_t mode);

    #include <sys/types.h>
    #include <sys/stat.h>
    int mkfifo(const char *pathname, mode_t mode);
        参数：
            - pathname: 管道名称的路径
            - mode: 文件的权限 和 open 的 mode 是一样的
                    是一个八进制的数
        返回值：成功返回0，失败返回-1，并设置错误号

*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h> 
#include <unistd.h>

int main(){

    int ret = access("fifo", F_OK);
    if(ret == -1){
        //printf("管道不存在，创建管道\n");
        perror("access");
        ret = mkfifo("fifo",0664);
        if(ret == -1){
            perror("mkfifo");
            exit(0);
        }
    }else{
        printf("管道已存在\n");
    }
    return 0;
}