//
// Created by 未名 on 2022/7/12.
//

#include "locker.h"


locker::locker() {
    if (pthread_mutex_init(&mutex, nullptr) != 0) {
        throw std::exception();
    }
}

locker::~locker() {
    pthread_mutex_destroy(&mutex);
}

bool locker::lock() {
    return pthread_mutex_lock(&mutex) == 0;
}

bool locker::unlock() {
    return pthread_mutex_unlock(&mutex) == 0;
}

condition::condition() {
    if (pthread_cond_init(&cond, nullptr) != 0) {
        throw std::exception();
    }
}

condition::~condition() {
    pthread_cond_destroy(&cond);
}

bool condition::wait(pthread_mutex_t *mutex) {
    return pthread_cond_wait(&cond, mutex) == 0;
}

bool condition::timewait(pthread_mutex_t *mutex, const struct timespec& t) {
    return pthread_cond_timedwait(&cond, mutex, &t) == 0;
}

bool condition::signal() {
    return pthread_cond_signal(&cond) == 0;
}

bool condition::broadcast() {
    return pthread_cond_broadcast(&cond) == 0;
}

semaphore::semaphore(int num) {
    if(sem_init(&sem, 0, num) != 0){
        throw std::exception();
    }
}

semaphore::~semaphore() {
    sem_destroy(&sem);
}

bool semaphore::wait() {
    return sem_wait(&sem) == 0;
}

bool semaphore::post() {
    return sem_post(&sem) == 0;
}
