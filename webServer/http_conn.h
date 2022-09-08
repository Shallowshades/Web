//
// Created by 未名 on 2022/7/11.
//

#ifndef VMSHARE_HTTP_CONN_H
#define VMSHARE_HTTP_CONN_H

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/uio.h>
#include <pthread.h>

#include <cstdarg>
#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>

#include "locker.h"

class http_conn {
public:

    static const int READ_BUFFER_SIZE = 2048;       //都缓冲区大小
    static const int WRITE_BUFFER_SIZE = 2048;      //写缓冲区大小
    static const int FILENAME_SIZE = 200;           // 文件名的最大长度

    // HTTP请求方法，这里只支持GET
    enum METHOD {GET = 0, POST, HEAD, PUT, DELETE, TRACE, OPTIONS, CONNECT};

    /*
        解析客户端请求时，主状态机的状态
        CHECK_STATE_REQUESTLINE :    当前正在分析请求行
        CHECK_STATE_HEADER      :    当前正在分析头部字段
        CHECK_STATE_CONTENT     :    当前正在解析请求体
    */
    enum CHECK_STATE { CHECK_STATE_REQUESTLINE = 0, CHECK_STATE_HEADER, CHECK_STATE_CONTENT };

    /*
        服务器处理HTTP请求的可能结果，报文解析的结果
        NO_REQUEST          :   请求不完整，需要继续读取客户数据
        GET_REQUEST         :   表示获得了一个完成的客户请求
        BAD_REQUEST         :   表示客户请求语法错误
        NO_RESOURCE         :   表示服务器没有资源
        FORBIDDEN_REQUEST   :   表示客户对资源没有足够的访问权限
        FILE_REQUEST        :   文件请求,获取文件成功
        INTERNAL_ERROR      :   表示服务器内部错误
        CLOSED_CONNECTION   :   表示客户端已经关闭连接了
    */
    enum HTTP_CODE { NO_REQUEST, GET_REQUEST, BAD_REQUEST, NO_RESOURCE, FORBIDDEN_REQUEST, FILE_REQUEST, INTERNAL_ERROR, CLOSED_CONNECTION };

    /*
     * 从状态机的三种可能状态，即行的读取状态，分别表示
     * 1.LINE_OK    :   读取到一个完整的行
     * 2.LINE_BAD   :   行出错
     * 3.LINE_OPEN  :   行数据尚且不完整
     */
    enum LINE_STATUS { LINE_OK = 0, LINE_BAD, LINE_OPEN };

    http_conn() = default;
    ~http_conn() = default;
    void init(int sockfd, const sockaddr_in& addr); //初始化新接受的连接
    void close_conn();                          //关闭连接
    void process();                             //处理客户端的请求
    bool read();                                //非阻塞的读
    bool write();                               //非阻塞的写

private:
    void init();                                //初始化连接其余的信息
    HTTP_CODE processRead();                    //解析HTTP请求
    bool processWrite(HTTP_CODE ret);                        //填充HTTP应答

    //这一组函数被processRead调用以分析HTTP请求
    HTTP_CODE parseRequestLine(char* text);     //解析HTTP请求行
    HTTP_CODE parseHeader(char* text);          //解析HTTP请求头
    HTTP_CODE parseContext(char* text);         //解析HTTP请求体
    HTTP_CODE doRequest();
    char* getLine() { return readBuffer + startLine; }
    LINE_STATUS parseLine(); //

    //这一组函数被processWrite调用以填充HTTP应答
    void unmap();
    bool addResponse(const char* format, ...);
    bool addContent(const char* content);
    bool addContentType();
    bool addStatusLine(int status, const char* title);
    bool addHeaders(int contentLength);
    bool addContentLength(int contentLength);
    bool addLinger();
    bool addBlankLine();

public:
    static int epollfd;                 //所有的socket事件都被注册到同一个epoll上
    static int userCount;               //统计用户的数量

private:
    int sockfd;                         //该HTTP连接的socket
    sockaddr_in address;                //该通信的socket地址
    char readBuffer[READ_BUFFER_SIZE];  //读缓冲区
    int readIndex;                      //标识缓冲区中已经读入客户端数据的最后一个字节的下一位
    int checkIndex;                     //当前正在分析字符在都缓冲区的位置
    int startLine;                      //当前正在解析的行的起始位置

    CHECK_STATE checkState;             //主状态机当前所处的状态
    METHOD method;                      //请求方法

    char realFile[FILENAME_SIZE];       //客户端请求的目标文件完整的路径名
    char* url;                          //请求目标文件名
    char* version;                      //协议版本，只支持HTTP1.1
    char* host;                         //主机名
    int contentLength;                  //HTTP请求的消息的总长度
    bool linger;                        //HTTP是否要保持连接

    char writeBuffer[WRITE_BUFFER_SIZE];//写缓冲区
    int writeIndex;                     //写缓冲区中待发送的字节数
    char* fileAddress;                  //客户端请求的目标文件被mmap到内存中的起始位置
    struct stat fileStat;               //目标文件的状态
    struct iovec iv[2];                 //采用writev来执行写操作，所以定义下面两个成员，其中ivCount表示被写内存块的数量
    int ivCount;                        //

    int bytesHaveSend;                  //已经发送的字节
    int bytesToSend;                    //将要发送的字节，写缓冲区将要发送的字节数
};


#endif //VMSHARE_HTTP_CONN_H
