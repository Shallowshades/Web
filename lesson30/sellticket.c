#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void* sell(void* arg){
    int tickets = 100;
    while(tickets > 0){
        usleep(3000);
        printf("child pthread id : %ld 正在售卖第%d张门票\n", pthread_self(), tickets--);
    }
    return NULL;
}

int main(){

    pthread_t tid1,tid2,tid3;
    pthread_create(&tid1,NULL,sell,NULL);
    pthread_create(&tid2,NULL,sell,NULL);
    pthread_create(&tid3,NULL,sell,NULL);

    //回收子线程资源
    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);
    pthread_join(tid3,NULL);

    //设置线程分离
    pthread_detach(tid1);
    pthread_detach(tid2);
    pthread_detach(tid3);

    //退出主线程
    pthread_exit(NULL);

    return 0;
}