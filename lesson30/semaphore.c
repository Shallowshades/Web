/*
    信号量的类型 sem_t
    int sem_init(sem_t *sem, int pshared, unsigned int value);
        - 初始化信号量
        - 参数：
            - sem : 信号量变量的地址
            - pshared : 0 用在线程间 ，非0用在进程间
            - value : 信号量中的值

    int sem_destroy(sem_t *sem);
        - 释放资源

    int sem_wait(sem_t *sem);
        - 对信号量加锁，调用一次对信号量的值-1，如果值为0，就阻塞

    int sem_trywait(sem_t *sem);

    int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);
    int sem_post(sem_t *sem);
        - 对信号量解锁，调用一次对信号量的值+1

    int sem_getvalue(sem_t *sem, int *sval);

    sem_t psem;
    sem_t csem;
    init(psem, 0, 8);
    init(csem, 0, 0);

    producer() {
        sem_wait(&psem);
        sem_post(&csem)
    }

    customer() {
        sem_wait(&csem);
        sem_post(&psem)
    }

*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h> 
//创建互斥量
pthread_mutex_t mutex;
//创建两个信号量
sem_t psem, csem;
//链表节点
struct node{
    int num;
    struct node* next;
};
//头结点
struct node* head = NULL;
//生产者
void* producer(void* arg){

    //不断创建节点，添加到链表中
    while(1){
        //信号量判断是否阻塞，减少信号量
        sem_wait(&psem);
        //加锁
        pthread_mutex_lock(&mutex);
        //头插法
        struct node* newnode = (struct node*)malloc(sizeof(struct node));
        newnode->next = head;
        head = newnode;
        newnode->num = rand() % 256;
        printf("add node, num : %d, tid : %ld\n", newnode->num, pthread_self());
        //解锁
        pthread_mutex_unlock(&mutex);
        //增加信号量
        sem_post(&csem);
        usleep(100);
    }
    return NULL;
}
//消费者
void* customer(void* arg){

    //不断的消费
    while(1){
        //
        sem_wait(&csem);
        //加锁
        pthread_mutex_lock(&mutex);
        //删除头结点
        struct node* tmp = head;
        head = head->next;
        printf("delete node num : %d, pid : %ld\n", tmp->num, pthread_self());
        free(tmp);
        //解锁
        pthread_mutex_unlock(&mutex);
        //
        sem_post(&psem);
    }
    return NULL;
}

int main(){

    //初始化互斥量
    pthread_mutex_init(&mutex,NULL);
    //初始化信号量
    sem_init(&psem, 0, 8);
    sem_init(&csem, 0, 0);
    //创建5个生产者线程，5个消费者线程
    pthread_t ptids[5], ctids[5];
    for(int i=0;i<5;++i){
        pthread_create(&ptids[i],NULL,producer,NULL);
        pthread_create(&ctids[i],NULL,customer,NULL);
    }
    //设置线程分离
    for(int i=0;i<5;++i){
        pthread_detach(ptids[i]);
        pthread_detach(ctids[i]);
    }

    //阻塞释放互斥量
    while(1){
        sleep(10);
    }
    //释放信号量资源
    sem_destroy(&psem);
    sem_destroy(&csem);
    //释放互斥量资源
    pthread_mutex_destroy(&mutex);

    //主线程退出
    pthread_exit(NULL);

    return 0;
}