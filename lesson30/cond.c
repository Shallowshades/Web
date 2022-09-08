/*
    条件变量的类型 pthread_cond_t
    int pthread_cond_init(pthread_cond_t *restrict cond, const pthread_condattr_t *restrict attr);
    int pthread_cond_destroy(pthread_cond_t *cond);
    int pthread_cond_wait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex);
        - 等待，调用了该函数，线程会阻塞。
    int pthread_cond_timedwait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex, const struct timespec *restrict abstime);
        - 等待多长时间，调用了这个函数，线程会阻塞，直到指定的时间结束。
    int pthread_cond_signal(pthread_cond_t *cond);
        - 唤醒一个或者多个等待的线程
    int pthread_cond_broadcast(pthread_cond_t *cond);
        - 唤醒所有的等待的线程
*/
/**
 * 生产者消费者模型
 */ 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h> 
//创建互斥量
pthread_mutex_t mutex;
//创建条件变量
pthread_cond_t cond;
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
        //加锁
        pthread_mutex_lock(&mutex);
        //头插法
        struct node* newnode = (struct node*)malloc(sizeof(struct node));
        newnode->next = head;
        head = newnode;
        newnode->num = rand() % 256;
        printf("add node, num : %d, pid : %ld\n", newnode->num, pthread_self());

        //只要生产了一个就通知消费者消费
        pthread_cond_signal(&cond);
        //pthread_cond_broadcast(&cond);

        //解锁
        pthread_mutex_unlock(&mutex);
        usleep(100);
    }
    return NULL;
}
//消费者
void* customer(void* arg){

    //不断的消费
    while(1){
        //加锁
        pthread_mutex_lock(&mutex);
        //删除头结点
        if(head != NULL){
            struct node* tmp = head;
            head = head->next;
            printf("delete node num : %d, pid : %ld\n", tmp->num, pthread_self());
            free(tmp);
        }else{
            //阻塞当前线程，等待signal或broadcast唤醒
            pthread_cond_wait(&cond, &mutex);
        }
        //解锁
        pthread_mutex_unlock(&mutex);
        //usleep(100); 
    }
    return NULL;
}

int main(){

    //初始化互斥量
    pthread_mutex_init(&mutex,NULL);
    //初始化条件变量
    pthread_cond_init(&cond,NULL);
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
    //释放条件变量
    pthread_cond_destroy(&cond);
    //释放互斥量资源
    pthread_mutex_destroy(&mutex);

    //主线程退出
    pthread_exit(NULL);

    return 0;
}