#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void* callback(void* arg){
    printf("child thread id: %ld\n",pthread_self());
    //sleep(3);
    //return NULL; //pthread_exit(NULL);
    int *value = malloc(sizeof(int));
    *value = 10;
    printf("child thread, value address: %u\n", (unsigned int)value);
    pthread_exit((void*)value); //return (void*)value;
}

int main(){

    pthread_t tid;

    int ret = pthread_create(&tid,NULL,callback,NULL);

    if(ret!=0){
        char *errstr = strerror(ret);
        printf("error: %s\n",errstr);
    }

    for(int i=0;i<5;++i){
        printf("%d\n",i);
    }

    printf("tid : %ld, main thread id : %ld\n", tid ,pthread_self());

    //回收子线程的资源
    int *thread_retval;
    ret = pthread_join(tid,(void**)&thread_retval);
    if(ret!=0){
        char *errstr = strerror(ret);
        printf("error: %s\n",errstr);
    }
    printf("exit data: %d\n", *thread_retval);
    printf("main thread, value address: %u\n", (unsigned int)thread_retval);
    free(thread_retval);
    printf("回收子线程资源成功！\n");

    //当主线程退出时不会影响其他进程的正常运行
    pthread_exit(NULL); 

    return 0;
}