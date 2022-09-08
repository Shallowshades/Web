#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <queue>
#include <errno.h>
#include <string.h>

using namespace std;

template<typename T>
class blockQueue{
public:
    blockQueue(){
        pthread_mutex_init(&mutex, nullptr);
        pthread_cond_init(&cond, nullptr);
    }

    ~blockQueue(){
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond);
    }

    T take(){
        pthread_mutex_lock(&mutex);
        while(q.empty()){
            pthread_cond_wait(&cond, &mutex);
        }
        T tmp = q.front();
        q.pop();
        pthread_mutex_unlock(&mutex);  
        return tmp;
    }

    void push(const T& val){
        pthread_mutex_lock(&mutex);
        q.push(val);
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);
    }

    blockQueue(const blockQueue&) = delete;
    
private:
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    queue<T> q;
};

blockQueue<int> b;

void* product(void* arg){
    for(int i = 0; i < 10; ++i){
        int x = rand() % 10000;
        b.push(x);
        printf("product thread id : %ld create num : %d\n", pthread_self(), x);
        usleep(50);
    }
}

void* custom(void* arg){
    int cnt = 0;
    while(1){
        int x = b.take();
        ++cnt;
        int sum = 0;
        for(int i = 0; i < x; ++i){
            sum += i;
        }
        usleep(100);
        printf("custom thread id : %ld calc num : %d sum : %d cnt = %d\n", pthread_self(), x, sum, cnt);
    }
}

int main(){

    pthread_t protid[4], custid[4];
    int ret = 0;
    for(int i = 0; i < 4; ++i){
        ret = pthread_create(&protid[i], nullptr, product, nullptr);
        if(ret != 0){
            char *errstr = strerror(ret);
            printf("error: %s\n", errstr);
        }

        ret = pthread_create(&custid[i], nullptr, custom, nullptr);
        if(ret != 0){
            char *errstr = strerror(ret);
            printf("error: %s\n", errstr);
        }
    }

    for(int i = 0; i < 4; ++i){
        pthread_join(protid[i], nullptr);//阻塞等待结束
    }

    sleep(2);

    for(int i = 0; i < 4; ++i){
        int ret = pthread_cancel(custid[i]);//强行终止
        if(ret != 0){
            printf("error %d\n", errno);
        }
    }

    printf("main thread is over\n");
    
    return 0;
}