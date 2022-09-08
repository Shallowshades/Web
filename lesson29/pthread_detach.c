#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void* callback(void* arg){
    printf("child thread: %ld\n", pthread_self());
    pthread_exit(NULL);
}
int main(){

    pthread_t tid;
    int ret = pthread_create (&tid,NULL,callback,NULL);
    if(ret!=0){
        char *errstr = strerror(ret);
        printf("error: %s\n",errstr);
    }

    //输出主线程和子线程的id
    printf("tid : %ld, main thread id : %ld\n", tid, pthread_self());

    //设置子线程分离
    ret = pthread_detach(tid);
    if(ret!=0){
        char *errstr = strerror(ret);
        printf("error: %s\n",errstr);
    }

    //分离后加入
    ret = pthread_join(tid, NULL);
    if(ret!=0){
        char *errstr = strerror(ret);
        printf("error: %s\n",errstr);
    }

    //主线程退出
    pthread_exit(NULL);

    return 0;
}
