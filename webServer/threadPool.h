//
// Created by 未名 on 2022/7/11.
//

/**
 * 线程池
 */
#ifndef VMSHARE_THREADPOOL_H
#define VMSHARE_THREADPOOL_H

#include <pthread.h>
#include <cstdio>
#include <list>
#include <exception>
#include "locker.h"

/**
 * 线程池类
 */
template<typename T>
class threadPool {
public:
    explicit threadPool(int threadNumber = 8, int maxRequests = 10000);
    ~threadPool();
    bool append(T *request);

private:
    //工作线程运行的函数，它不断从工作队列中取出任务并执行之
    static void* worker(void* arg);
    void run();

private:
    int threadNumber;           //线程的数量
    pthread_t *threads;         //线程池数组
    int maxRequests;            //请求队列中最多允许的等待处理的请求的数量
    std::list<T *> workQueue;   //请求队列
    locker queueLocker;         //互斥锁
    semaphore queueStat;        //信号量 用于判断是否有任务需要处理
    bool stop;                  //是否结束线程
};

template<typename T>
threadPool<T>::threadPool(int threadNumber, int maxRequests) :
        threadNumber(threadNumber), maxRequests(maxRequests),
        stop(false), threads(nullptr) {

    if (threadNumber <= 0 || maxRequests <= 0) {
        throw std::exception();
    }

    this->threads = new pthread_t[threadNumber];
    if (this->threads == nullptr) {
        throw std::exception();
    }

    //创建线程，并且设置线程分离
    for (int i = 0; i < threadNumber; ++i) {
        //printf("create the %dth thread\n", i);

        if (pthread_create(this->threads + i, nullptr, worker, this) != 0) {
            delete[] this->threads;
            throw std::exception();
        }

        if (pthread_detach(this->threads[i])) {
            delete[] this->threads;
            throw std::exception();
        }
    }

}


template<typename T>
threadPool<T>::~threadPool() {
    delete[] this->threads;
    this->stop = true;
}

template<typename T>
bool threadPool<T>::append(T *request) {
    // 操作工作队列时一定要加锁，因为它被所有线程共享。
    this->queueLocker.lock();
    if(this->workQueue.size() > this->maxRequests){
        queueLocker.unlock();
        return false;
    }

    workQueue.push_back(request);
    queueLocker.unlock();
    queueStat.post();
    return true;
}

template<typename T>
void *threadPool<T>::worker(void *arg) {
    auto* pool = (threadPool*)arg;
    pool->run();
    return pool;
}

template<typename T>
void threadPool<T>::run() {
    while(!this->stop){
        queueStat.wait();
        queueLocker.lock();
        if(workQueue.empty()){
            queueLocker.unlock();
            continue;
        }

        T* request = workQueue.front();
        workQueue.pop_front();
        queueLocker.unlock();

        if(request == nullptr){
            continue;
        }

        request->process();
    }
}

#endif //VMSHARE_THREADPOOL_H
