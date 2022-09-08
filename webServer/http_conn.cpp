//
// Created by 未名 on 2022/7/11.
//

#include "http_conn.h"

// 定义HTTP响应的一些状态信息
const char *ok_200_title = "OK";
const char *error_400_title = "Bad Request";
const char *error_400_form = "Your request has bad syntax or is inherently impossible to satisfy.\n";
const char *error_403_title = "Forbidden";
const char *error_403_form = "You do not have permission to get file from this server.\n";
const char *error_404_title = "Not Found";
const char *error_404_form = "The requested file was not found on this server.\n";
const char *error_500_title = "Internal Error";
const char *error_500_form = "There was an unusual problem serving the requested file.\n";

//网站的根目录
const char *documentRoot = "/home/shallow/Linux/webServer/resources";

//设置文件描述符非阻塞
int setnonblocking(int fd) {
    int old_flag = fcntl(fd, F_GETFL);
    int new_flag = old_flag | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_flag);
    return old_flag;
}

//添加文件描述符到epoll
//在ep_send_events_proc()中，会检查epitem是不是oneshot模式的，
//如果是的，第一次唤醒还是正常处理的，但是会将epitem中监听的事件选项清0，
//那么后续唤醒的时候，执行ep_item_poll()获取监听事件的时候，就获取的是空事件的。
//即确保了该监听项的oneshot。
void addfd(int epollfd, int fd, bool one_shot) {
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLRDHUP;
    //event.events = EPOLLIN | EPOLLRDHUP | EPOLLET;

    

    //防止同一个通信被不同的线程处理
    if (one_shot) {
        event.events |= EPOLLONESHOT;
    }

    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    //设置文件描述符非阻塞
    setnonblocking(fd);
}

//从epoll中删除文件描述符
void removefd(int epollfd, int fd) {
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
    close(fd);
}

//修改epoll中的文件描述符 重置socket上EPOLLONESHOT事件，以确保下一次可读，EPOLLIN事件能被触发
void modfd(int epollfd, int fd, int ev) {
    epoll_event event;
    event.data.fd = fd;
    event.events = ev | EPOLLET | EPOLLONESHOT | EPOLLRDHUP;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

int http_conn::epollfd = -1;    //所有的socket上的事件都被注册到同一个epoll内核事件中，所以设置成静态
int http_conn::userCount = 0;   //所有的客户端数量

//关闭连接
void http_conn::close_conn() {
    if (this->sockfd != -1) {
        removefd(http_conn::epollfd, sockfd);
        sockfd = -1;
        --userCount;
    }
}

//初始化连接，外部调用初始化套接字地址
void http_conn::init(int sockfd, const sockaddr_in &addr) {
    this->sockfd = sockfd;
    this->address = addr;

    //端口复用
    int reuse = 1;
    setsockopt(this->sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    //添加到epoll对象中
    addfd(http_conn::epollfd, this->sockfd, true);
    ++http_conn::userCount;

    init();
}

//内部调用
void http_conn::init() {
    bytesToSend = 0;
    bytesHaveSend = 0;

    checkState = CHECK_STATE_REQUESTLINE; //初始化状态为解析请求首行
    linger = false; //默认不保持连接 Connection : keep-alive保持连接
    method = GET;
    url = nullptr;
    version = nullptr;
    contentLength = 0;
    host = nullptr;
    startLine = 0;
    checkIndex = 0;
    readIndex = 0;
    writeIndex = 0;
    bzero(readBuffer, READ_BUFFER_SIZE);
    bzero(writeBuffer, WRITE_BUFFER_SIZE);
    bzero(realFile, FILENAME_SIZE);
}

//读数据
bool http_conn::read() {
    //printf("一次性读完数据\n");
    if (readIndex >= READ_BUFFER_SIZE) {
        return false;
    }

    //读取到的字节
    int bytesRead = 0;
    while (true) {
        // 从readBuffer+readIndex索引出开始保存数据，大小是READ_BUFFER_SIZE - readIndex
        bytesRead = recv(sockfd, readBuffer + readIndex, READ_BUFFER_SIZE - readIndex, 0);
        if (bytesRead == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                //没有数据
                break;
            }
            //出错
            return false;
        } else if (bytesRead == 0) {
            //对方关闭连接
            return false;
        }
        //将读取指针后移
        readIndex += bytesRead;
    }
    //printf("读取到了数据:\n %s\n", readBuffer);
    return true;
}

//解析一行，判断依据\r\n 把一行分开'\0'
http_conn::LINE_STATUS http_conn::parseLine() {
    char temp;
    for (; checkIndex < readIndex; ++checkIndex) {
        temp = readBuffer[checkIndex];
        if (temp == '\r') {
            if (checkIndex + 1 == readIndex) {
                return LINE_OPEN; //未读完
            } else if (readBuffer[checkIndex + 1] == '\n') {
                readBuffer[checkIndex++] = '\0';
                readBuffer[checkIndex++] = '\0';
                return LINE_OK;//读完
            }
            return LINE_BAD;//行出错
        } else if (temp == '\n') {
            //上一次读取到\r,此次开始读到\n
            if (checkIndex > 1 && readBuffer[checkIndex - 1] == '\r') {
                readBuffer[checkIndex - 1] = '\0';
                readBuffer[checkIndex++] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        }
    }
    return LINE_OK;
}

//解析HTTP请求行 请求方法 目标URL HTTP版本号
http_conn::HTTP_CODE http_conn::parseRequestLine(char *text) {
    //GET /index.html HTTP/1.1
    //判断第二个参数中的字符，哪个在第一个参数中最先出现
    url = strpbrk(text, " \t");
    if (url == nullptr) {
        return BAD_REQUEST;
    }
    //GET\0/index.html HTTP/1.1
    *url++ = '\0';
    char *methodStr = text;
    //strcasecmp大小写不敏感对比
    if (strcasecmp(methodStr, "GET") == 0) {
        method = GET;
    } else {
        return BAD_REQUEST;
    }
    //  /index.html HTTP/1.1
    version = strpbrk(url, " \t");
    if (version == nullptr) {
        return BAD_REQUEST;
    }
    *version++ = '\0';
    if (strcasecmp(version, "HTTP/1.1") != 0) {
        return BAD_REQUEST;
    }

    //http://192.168.29.135:10000/index.html
    // -> /index.html
    if (strncasecmp(url, "http://", 7) == 0) {
        url += 7;
        //在参数s所指向的字符串中搜索第一次出现字符c的位置。
        url = strchr(url, '/');
    }

    if (url == nullptr || url[0] != '/') {
        return BAD_REQUEST;
    }

    //主状态机检查状态变成检查请求头
    checkState = CHECK_STATE_HEADER;
    return http_conn::NO_REQUEST;
}

http_conn::HTTP_CODE http_conn::parseHeader(char *text) {
    //遇到空行表示头部字段解析完毕
    if (text[0] == '\0') {
        //如果HTTP请求有消息体，则还需要读取contentLength字节的请求体
        //状态机转移到CHECK_STATE_CONTENT
        if (contentLength != 0) {
            checkState = CHECK_STATE_CONTENT;
            return NO_REQUEST;
        }
        //否则说明已经得到一个完整的HTTP请求
        return GET_REQUEST;
    } else if (strncasecmp(text, "Connection:", 11) == 0) {
        //处理Connection头部字段 Connection: keep-alive
        text += 11;
        //函数返回字符串s中第一个不包含于字符列表accept中的字符的索引 去除头部的' '和'\t'
        text += strspn(text, " \t");
        if (strcasecmp(text, "keep-alive") == 0) {
            linger = true;
        }
    } else if (strncasecmp(text, "Content-Length:", 15) == 0) {
        //处理Content-Length头部字段
        text += 15;
        text += strspn(text, " \t");
        //contentLength = atol(text);
        contentLength = strtol(text, &text + strlen(text), 10);
    } else if (strncasecmp(text, "Host:", 5) == 0) {
        //处理Host头部字段
        text += 5;
        text += strspn(text, " \t");
        host = text;
    } else {
        //printf("oop! unkown header %s\n", text);
    }
    return http_conn::NO_REQUEST;
}

http_conn::HTTP_CODE http_conn::parseContext(char *text) {
    //请求体内容已读取
    if (readIndex >= contentLength + checkIndex) {
        text[contentLength] = '\0';
        return GET_REQUEST;
    }
    return http_conn::NO_REQUEST;
}

//主状态机，解析请求
http_conn::HTTP_CODE http_conn::processRead() {
    LINE_STATUS lineStatus = LINE_OK;
    HTTP_CODE ret = NO_REQUEST;
    char *text = nullptr;

    while ((checkState == CHECK_STATE_CONTENT && lineStatus == LINE_OK)
           || ((lineStatus = parseLine()) == LINE_OK)) {
        //获取一行数据
        text = getLine();
        startLine = checkIndex;
        //printf("got 1 http line : %s\n", text);

        switch (checkState) {
            case CHECK_STATE_REQUESTLINE : {
                ret = parseRequestLine(text);
                if (ret == BAD_REQUEST) {
                    return BAD_REQUEST;
                }
                break;
            }

            case CHECK_STATE_HEADER : {
                ret = parseHeader(text);
                if (ret == BAD_REQUEST) {
                    return BAD_REQUEST;
                } else if (ret == GET_REQUEST) {
                    return doRequest();//解析完HTTP请求
                }
                break;
            }

            case CHECK_STATE_CONTENT : {
                ret = parseContext(text);
                if (ret == GET_REQUEST) {
                    return doRequest();
                }
                lineStatus = LINE_OPEN;
                break;
            }

            default : {
                return INTERNAL_ERROR;
            }
        }
    }
    return http_conn::NO_REQUEST;
}

// 当得到一个完整、正确的HTTP请求时，分析目标文件的属性，
// 如果目标文件存在、对所有用户可读，且不是目录，则使用mmap将其
// 映射到内存地址fileAddress处，并告诉调用者获取文件成功
http_conn::HTTP_CODE http_conn::doRequest() {
    //"/home/shallow/Linux/webServer/resource";
    strcpy(realFile, documentRoot);
    int len = strlen(documentRoot);
    strncpy(realFile + len, url, FILENAME_SIZE - len - 1);
    //获取realFile文件的相关状态信息，-1失败，0成功
    if (stat(realFile, &fileStat) < 0) {
        return NO_RESOURCE;
    }

    //判断访问权限
    if (!(fileStat.st_mode & S_IROTH)) {
        return FORBIDDEN_REQUEST;
    }
    //判断是否时目录
    if (S_ISDIR(fileStat.st_mode)) {
        return BAD_REQUEST;
    }
    //以只读的方式打开
    int fd = open(realFile, O_RDONLY);
    //创建内存映射
    fileAddress = (char *) mmap(0, fileStat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    return http_conn::FILE_REQUEST;
}

//对内存映射区执行munmap操作
void http_conn::unmap() {
    if (fileAddress != nullptr) {
        munmap(fileAddress, fileStat.st_size);
        fileAddress = nullptr;
    }
}

//写HTTP响应
bool http_conn::write() {
    int tmp = 0;

    //将要发送的字节为0，这一次响应结束
    if (bytesToSend == 0) {
        modfd(epollfd, sockfd, EPOLLIN);
        init();
        return true;
    }

    while (1) {
        //分散写
        tmp = writev(sockfd, iv, ivCount);
        if (tmp <= -1) {
            // 如果TCP写缓冲没有空间，则等待下一轮EPOLLOUT事件，
            // 虽然在此期间，服务器无法立即接收到同一客户的下一个请求，但可以保证连接的完整性。
            if (errno == EAGAIN) {
                modfd(epollfd, sockfd, EPOLLOUT);
                return true;
            }
            unmap();
            return false;
        }
        bytesToSend -= tmp;
        bytesHaveSend += tmp;

        
        if(bytesHaveSend >= iv[0].iov_len){
            //头已经发送完毕
            iv[0].iov_len = 0;
            iv[1].iov_base = fileAddress + (bytesHaveSend - writeIndex);
            iv[1].iov_len = bytesToSend;
        }else{
            iv[0].iov_base = writeBuffer + bytesHaveSend;
            iv[0].iov_len = iv[0].iov_len - tmp;
        }

        if (bytesToSend <= 0) {
            // 发送HTTP响应成功，根据HTTP请求中的Connection字段决定是否立即关闭连接
            unmap();
            modfd(epollfd, sockfd, EPOLLIN);
            if (linger) {
                init();
                return true;
            } else {
                return false;
            }
        }
    }
}

//往写缓冲区写入待发送的数据
bool http_conn::addResponse(const char *format, ...) {
    if (writeIndex >= WRITE_BUFFER_SIZE) {
        return false;
    }
    va_list argList;
    va_start(argList, format);
    int len = vsnprintf(writeBuffer + writeIndex, WRITE_BUFFER_SIZE - 1 - writeIndex, format, argList);
    if (len >= (WRITE_BUFFER_SIZE - 1 - writeIndex)) {
        return false;
    }
    writeIndex += len;
    va_end(argList);
    return true;
}

bool http_conn::addStatusLine(int status, const char *title) {
    return addResponse("%s %d %s\r\n", "HTTP/1.1", status, title);
}

bool http_conn::addHeaders(int contentLength) {
    addContentLength(contentLength);
    addContentType();
    addLinger();
    addBlankLine();
    //return true;
}

bool http_conn::addContentLength(int contentLength) {
    return addResponse("Content-Length: %d\r\n", contentLength);
}

bool http_conn::addLinger() {
    return addResponse("Connection: %s\r\n", (linger == true) ? "keep-alive" : "close");
}

bool http_conn::addBlankLine() {
    return addResponse("%s", "\r\n");
}

bool http_conn::addContent(const char *content) {
    return addResponse("%s", content);
}

bool http_conn::addContentType() {
    return addResponse("Content-Type:%s\r\n", "text/html");
}

//根据处理的结果，返回客户端的内容
bool http_conn::processWrite(HTTP_CODE ret) {

    //printf("%d\n", ret);
    switch (ret) {
        case INTERNAL_ERROR : {
            addStatusLine(500, error_500_title);
            addHeaders(strlen(error_500_form));
            if (!addContent(error_500_form)) {
                return false;
            }
            break;
        }

        case BAD_REQUEST : {
            addStatusLine(400, error_400_title);
            addHeaders(strlen(error_400_form));
            if (!addContent(error_400_form)) {
                return false;
            }
            break;
        }

        case NO_RESOURCE : {
            addStatusLine(404, error_404_title);
            addHeaders(strlen(error_404_form));
            if (!addContent(error_404_form)) {
                return false;
            }
            break;
        }

        case FORBIDDEN_REQUEST : {
            addStatusLine(403, error_403_title);
            addHeaders(strlen(error_403_form));
            if (!addContent(error_403_form)) {
                return false;
            }
            break;
        }

        case FILE_REQUEST : {
            addStatusLine(200, ok_200_title);
            addHeaders(fileStat.st_size);
            iv[0].iov_base = writeBuffer;
            iv[0].iov_len = writeIndex;
            iv[1].iov_base = fileAddress;
            iv[1].iov_len = fileStat.st_size;
            ivCount = 2;

            bytesToSend = writeIndex + fileStat.st_size;

            return true;
        }

        default : {
            return false;
        }
    }

    iv[0].iov_base = writeBuffer;
    iv[0].iov_len = writeIndex;
    ivCount = 1;
    bytesToSend = writeIndex;
    return true;
}

//由线程池中的工作线程调用，这是处理HTTP请求的入口函数
void http_conn::process(){
    //解析HTTP请求
    //printf("parse request, create response\n");
    HTTP_CODE readRet = processRead();
    if (readRet == NO_REQUEST) {
        modfd(epollfd, sockfd, EPOLLIN);
        return;
    }
    //生成响应
    bool writeRet = processWrite(readRet);
    if (!writeRet) {
        close_conn();
    }
    modfd(epollfd, sockfd, EPOLLOUT);
}
