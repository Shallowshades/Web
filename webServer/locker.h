//
// Created by 未名 on 2022/7/10.
//
/**
 * 线程同步机制封装类
 */
#ifndef VMSHARE_LOCKER_H
#define VMSHARE_LOCKER_H

#include <pthread.h>
#include <exception>
#include <semaphore.h>

/**
 * 互斥锁类
 */
class locker {
public:
    locker();
    ~locker();

    bool lock() ;
    bool unlock() ;

    pthread_mutex_t* get() { return &mutex; }

private:
    pthread_mutex_t mutex;
};

/**
 * 条件变量类
 */
class condition{
public:
    condition();
    ~condition();

    bool wait(pthread_mutex_t* mutex);
    bool timewait(pthread_mutex_t* mutex, const struct timespec& t);

    bool signal();
    bool broadcast();
private:
    pthread_cond_t cond;
};

/**
 * 信号量类
 */
class semaphore{
public:
    explicit semaphore(int num = 0);
    ~semaphore();

    /**
     *
     * @return
     */
    bool wait();
    bool post();

private:
    sem_t sem;
};

#endif //VMSHARE_LOCKER_H
