#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void* callback(void* arg){
    printf("child thread id: %ld\n",pthread_self());
    return NULL; //pthread_exit(NULL);
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

    printf("main thread id: %ld\n",pthread_self());

    pthread_exit(NULL); //当主线程退出时不会影响其他进程的正常运行

    return 0;
}