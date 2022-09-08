#include <unistd.h>
#include <stdio.h>
#include <sys/epoll.h>

int main(){
    int epfd, nfds;
    struct epoll_event event, events[5];
    epfd = epoll_create(1);
    event.data.fd = STDIN_FILENO;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &event);
    while (1) {
        nfds = epoll_wait(epfd, events, 5, -1);
        int i;
        for (i = 0; i < nfds; ++i) {
            printf("i = %d\n",i);
            if (events[i].data.fd == STDIN_FILENO) {
                printf("hello world\n");
                event.data.fd = STDIN_FILENO;
                event.events = EPOLLIN | EPOLLET;
                //猜测为在红黑树中插入相同时，直接丢弃，也就是没有插入新的，也没有改变旧的，不会再次触发事件
                epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &event); //有输入时输出
                //修改了原有的文件描述符时，会检查一次有无数据，大概吧  
                //epoll_ctl(epfd, EPOLL_CTL_MOD, STDIN_FILENO, &event); //死循环输出
            }
        }
    }
}