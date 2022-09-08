/*
    // 打开一个目录
    #include <sys/types.h>
    #include <dirent.h>
    DIR *opendir(const char *name);
        参数：
            - name: 需要打开的目录的名称
        返回值：
            DIR * 类型，理解为目录流
            错误返回NULL


    // 读取目录中的数据
    #include <dirent.h>
    struct dirent *readdir(DIR *dirp);
        - 参数：dirp是opendir返回的结果
        - 返回值：
            struct dirent，代表读取到的文件的信息
            读取到了末尾或者失败了，返回NULL

    // 关闭目录
    #include <sys/types.h>
    #include <dirent.h>
    int closedir(DIR *dirp);

*/

/*

1. 
    gcc readFileNum.c -D_BSD_SOURCE -o readFileNum --std=c99
2.
    #ifndef __USE_MISC
    #define __USE_MISC	1
    #endif
3.
    DT_UNKNOWN  =   0,     0000     //未知类型       
    DT_FIFO     =   1,     0001     //管道文件
    DT_CHR      =   2,     0010     //字符设备文件 
    DT_DIR      =   4,     0100     //目录 
    DT_BLK      =   6,     0110     //块设备文件 
    DT_REG      =   8,     1000     //普通文件 
    DT_LNK      =   10,    1010     //连接文件 
    DT_SOCK     =   12,    1100     //套接字类型 
    DT_WHT      =   14     1110     //    

 */
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

// #ifndef __USE_MISC
// #define __USE_MISC	1
// #endif

int getFileNum(const char * path);

// 读取某个目录下所有的普通文件的个数
int main(int argc, char * argv[]) {

    if(argc < 2) {
        printf("%s path\n", argv[0]);
        return -1;
    }

    int num = getFileNum(argv[1]);

    printf("普通文件的个数为：%d\n", num);

    return 0;
}

// 用于获取目录下所有普通文件的个数
int getFileNum(const char * path) {

    // 1.打开目录
    DIR * dir = opendir(path);

    if(dir == NULL) {
        perror("opendir");
        exit(0);
    }

    struct dirent *ptr;

    // 记录普通文件的个数
    int total = 0;

    while((ptr = readdir(dir)) != NULL) {

        // 获取名称
        char * dname = ptr->d_name;

        // 忽略掉. 和..
        if(strcmp(dname, ".") == 0 || strcmp(dname, "..") == 0) {
            continue;
        }

/*
        DT_UNKNOWN  =   0,     0000     //未知类型       
        DT_FIFO     =   1,     0001     //管道文件
        DT_CHR      =   2,     0010     //字符设备文件 
        DT_DIR      =   4,     0100     //目录 
        DT_BLK      =   6,     0110     //块设备文件 
        DT_REG      =   8,     1000     //普通文件 
        DT_LNK      =   10,    1010     //连接文件 
        DT_SOCK     =   12,    1100     //套接字类型 
        DT_WHT      =   14     1110     //
*/
        // 判断是否是普通文件还是目录
        if(ptr->d_type == 4) {
            // 目录,需要继续读取这个目录
            char newpath[256];
            sprintf(newpath, "%s/%s", path, dname);
            total += getFileNum(newpath);
        }

        if(ptr->d_type == 8) {
            // 普通文件
            total++;
        }


    }

    // 关闭目录
    closedir(dir);

    return total;
}