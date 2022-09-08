/**
 * 生产者消费者模型
 */ 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h> 
//创建互斥量
pthread_mutex_t mutex;
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
        printf("add node, num : %d, tid : %ld\n", newnode->num, pthread_self());
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
        }
        //解锁
        pthread_mutex_unlock(&mutex);
        usleep(100); 
    }
    return NULL;
}

int main(){

    //初始化互斥量
    pthread_mutex_init(&mutex,NULL);
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
    //释放互斥量资源
    pthread_mutex_destroy(&mutex);

    //主线程退出
    pthread_exit(NULL);

    return 0;
}