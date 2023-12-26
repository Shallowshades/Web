#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>

#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/epoll.h>
#include <signal.h>

#include "locker.h"
#include "threadPool.h"
#include "http_conn.h"

#define MAX_FD 65535 //最大的文件描述符个数
#define MAX_EVENT 10000 //最大的监听数量

//添加文件描述符到epoll
extern void addfd(int epollfd, int fd, bool one_shot);
//从epoll中删除文件描述符
extern void removefd(int epollfd, int fd);
//修改epoll中的文件描述符
extern void modfd(int epollfd, int fd, int ev);
//添加信号捕捉
void addSig(int sig, void(handler)(int)) {
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = handler;
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig, &sa, nullptr) != -1);
}

int main(int argc, char* argv[]) {

    if (argc <= 1) {
        printf("按照如下格式运行： %s port_number\n", basename(argv[0]));
        exit(-1);
    }

    //获取端口号
    unsigned int port = strtol(argv[1], &argv[1] + strlen(argv[1]), 10);
    //int port = atoi(argv[1]);

//    printf("port = %u\n", port);
//    exit(-1);

    //对SIGPIPE信号进行处理
    addSig(SIGPIPE, SIG_IGN);

    //创建线程池，初始化线程池
    threadPool<http_conn>* pool = nullptr;
    try {
        pool = new threadPool<http_conn>;
    }
    catch (...) {
        exit(-1);
    }

    //创建一个数组，用于保存所有客户端的信息
    http_conn* users = new http_conn[MAX_FD];

    //创建监听的套接字
    //PF protocol family 协议族
    //AF address family 地址族
    int listenfd = socket(PF_INET, SOCK_STREAM, 0);

    //设置端口复用 在绑定之前设置
    int reuse = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    //绑定
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);
    int ret = bind(listenfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (ret == -1) {
        perror("bind");
        exit(-1);
    }

    //监听
    ret = listen(listenfd, 128);
    if (ret == -1) {
        perror("listen");
        exit(-1);
    }

    //创建epoll对象，事件数组，添加
    epoll_event events[MAX_EVENT];
    int epollfd = epoll_create(5);

    //将监听的文件描述符添加到epoll对象中
    addfd(epollfd, listenfd, false);//非阻塞
    http_conn::epollfd = epollfd;

    while (true) {
        int num = epoll_wait(epollfd, events, MAX_EVENT, -1);
        if (num < 0 && errno != EINTR) {
            printf("epoll failure\n");
            break;
        }

        //循环遍历事件数组
        for (int i = 0;i < num;++i) {
            int sockfd = events[i].data.fd;
            if (sockfd == listenfd) {
                //有新的客户端连接进来
                struct sockaddr_in clientAddr;
                socklen_t clientAddrLen = sizeof(clientAddr);
                //accept阻塞的条件是没有一个完成了三次握手的连接。
                //accept就是从队列里取节点，每个节点都是已经完成了三次握手的。
                int connfd = accept(listenfd, (struct sockaddr*)&clientAddr, &clientAddrLen);
                if (connfd == -1) {
                    printf("errno is : %d\n", errno);
                    continue;
                }

                //目前连接满了, 给客户端写一个信息，服务器内部正忙
                if (http_conn::userCount >= MAX_FD) {
                    close(connfd);
                    continue;
                }

                //将新的客户端的数据初始化，放到数组中
                users[connfd].init(connfd, clientAddr);
            }
            else if (events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
                //对方异常断开或者错误事件
                users[sockfd].close_conn();
            }
            else if (events[i].events & EPOLLIN) {
                if (users[sockfd].read()) {
                    //一次性将所有数据都读完
                    pool->append(users + sockfd);
                }
                else {
                    users[sockfd].close_conn();
                }
            }
            else if (events[i].events & EPOLLOUT) {
                if (!users[sockfd].write()) {
                    //一次性写完所有数据
                    users[sockfd].close_conn();
                }
            }
        }
    }

    close(epollfd);
    close(listenfd);
    delete[] users;
    delete pool;

    //printf("Hello, WebServer\n");

    return 0;
}
