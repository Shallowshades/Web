#include <unistd.h>
#include <stdio.h>

int main(){

    int ret = alarm(5);
    printf("ret = %d\n", ret);

    sleep(2);

    ret = alarm(2);
    printf("ret = %d\n", ret);

    while(1){
        
    }

    return 0;
}