#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void *callback(void* arg){
    printf("child pthread...\n");
    printf("arg: %d\n", *(int *)arg);
    return NULL;
}

int main(){

    pthread_t tid;
    
    int num = 10;
    
    int ret = pthread_create(&tid,NULL,callback,(void*)&num);

    if(ret!=0){
        char *errstr = strerror(ret);
        printf("error: %s\n", errstr);
    }

    for(int i=0;i<5;++i){
        printf("%d\n",i);
    }

    sleep(1);


    return 0;
}