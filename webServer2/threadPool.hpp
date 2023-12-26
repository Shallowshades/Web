/**
 *@file threadPool.h
 * @author your name (you@domain.com)
 * @brief 线程池
 * @version 0.1
 * @date 2023-12-26
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <thread>
#include <iostream>
#include <list>
#include <exception>
#include <mutex>
#include <condition_variable>
#include <vector>

 /// @brief 线程池类
 /// @tparam T 
template<typename T>
class threadPool {
public:
    /// @brief 构造函数
    /// @param threadNumber 线程数量（默认-8）
    /// @param maxRequests 最大请求数量（默认-10000）
    explicit threadPool(int threadNumber = 8, int maxRequestNumber = 10000);

    /// @brief 将请求挂起
    /// @param request http连接请求
    /// @return 
    bool append(T* request);

    ~threadPool();

private:
    /// @brief 
    void run();

private:

    int m_iThreadNumber;    //线程数量
    int m_iMaxRequestNumber;    //最大请求数量
    std::vector<std::thread*> m_threadsArray; //线程数组
    std::list<T*> m_listWorkQueue;  //请求队列
    std::mutex m_mtxQueueLock;  //请求队列互斥锁
    std::condition_variable m_cvQueueStatus;    //请求队列条件变量
    bool m_bThreadStop;  //是否结束线程
};

template<typename T>
threadPool<T>::threadPool(int threadNumber, int maxRequestNumber)
    : m_iThreadNumber(threadNumber),
    m_iMaxRequestNumber(maxRequestNumber),
    m_bThreadStop(false),
    m_threadsArray(nullptr) {

    if (m_iThreadNumber <= 0 || m_iMaxRequestNumber <= 0)
        throw std::exception("请输入合理的线程数和最大请求数");

    //创建线程，并且设置线程分离
    m_threadsArray.reserve(m_iThreadNumber);
    for (int i = 0;i < m_iThreadNumber; ++i) {
        m_threadsArray[i] = new std::thread(run);
        if (m_threadsArray[i] == nullptr)
            throw std::exception("线程分配内存时出错");

        if (!m_threadsArray[i]->joinable()) {
            delete m_threadsArray[i];
            throw std::exception("线程分离失败");
        }
        m_threadsArray[i]->detach();
    }
}

template<typename T>
bool threadPool<T>::append(T* request) {

    //临界区加锁
    //m_mtxQueueLock.lock();
    std::lock_guard<std::mutex> locker(m_mtxQueueLock);
    if (m_listWorkQueue.size() > this->m_iMaxRequestNumber)
        return false;

    //TODO: 此处这样是否影响性能
    m_listWorkQueue.push_back(request);
    m_cvQueueStatus.notify_all();
    return true;
}

template<typename T>
threadPool<T>::~threadPool() {
    for (auto&& elem : m_threadsArray)
        delete elem;

    m_bThreadStop = true;
}

template<typename T>
void threadPool<T>::run() {
    while (!m_bThreadStop) {
        m_cvQueueStatus.wait();
        m_mtxQueueLock.lock();
        if (m_listWorkQueue.empty()) {
            m_mtxQueueLock.unlock();
            continue;
        }

        T* request = m_listWorkQueue.front();
        m_listWorkQueue.pop_front();
        m_mtxQueueLock.unlock();

        if (request == nullptr)
            continue;

        //
        //request->process();
    }
}