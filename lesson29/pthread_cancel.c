#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void* callback(void* arg){
    printf("child thread: %ld\n", pthread_self());
    for(int i=0;i<50000;++i){
        printf("child thread : %d\n",i);
    }
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

    //子线程退出 并不会立马退出，而是到一个退出点（可以认为由系统调用的时候，转换到内核区）
    ret = pthread_cancel(tid); 
    if(ret!=0){
        char *errstr = strerror(ret);
        printf("error: %s\n",errstr);
    }

    for(int i=0;i<5;++i){
        printf("%d\n",i);
    }

    //主线程退出
    pthread_exit(NULL);

    return 0;
}
