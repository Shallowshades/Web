/**
 * @file alarm1.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-06-26
 * 
 * @copyright Copyright (c) 2022
 * 
 * 1s pc能计多少个数
 */

#include <stdio.h>
#include <unistd.h>

/*

    实际时间 = 内核时间 + 用户时间 + I/O等消耗的时间
    I/O比较耗时

    定时器与进程的状态无关（自然定时法）。无论进程处于什么状态alarm都会计时。

*/

int main(){

    alarm(1);
    for(int i=1;;++i){
        printf("%d\n",i);
    }

    return 0;
}