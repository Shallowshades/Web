#include <unistd.h>
#include <stdio.h>

int main(){

    int ret = rmdir("aaa");
    if(ret == -1){
        perror("rmdir");
        return -1;
    }

    printf("文件夹删除成功\n");

    return 0;
}