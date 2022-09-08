/*
    #include <stdlib.h>
    void exit(int status);

    #include <unistd.h>
    void _exit(int status);

    status参数：是进程退出时的一个状态信息。父进程回收子进程资源的时候可以获取到。
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {

    printf("hello\n");
    printf("world");

    //exit(0); //C中的函数，在退出前做了很多处理，例如调用退出处理函数，刷新缓冲区
    _exit(0); //linux的系统函数，直接退出，没有刷新缓冲区
    
    return 0;
}