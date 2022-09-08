/**
 * @file setitimer.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-06-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/*
NAME
    getitimer, setitimer - get or set value of an interval timer

SYNOPSIS
    #include <sys/time.h>

    int getitimer(int which, struct itimerval *curr_value);
    int setitimer(int which, const struct itimerval *new_value,
                                    struct itimerval *old_value);
                
                //new_value用于设置新参数
                //old_value一般用于传出旧参数

RETURN VALUE
    On success, zero is returned.  
    On error, -1 is returned, and errno is set appropriately.

    - 功能：设置定时器（闹钟）。可以替代alarm函数。精度微妙us，可以实现周期性定时
    - 参数：
        - which : 定时器以什么时间计时
            ITIMER_REAL: 真实时间，时间到达，发送 SIGALRM   常用
            ITIMER_VIRTUAL: 用户时间，时间到达，发送 SIGVTALRM
            ITIMER_PROF: 以该进程在用户态和内核态下所消耗的时间来计算，时间到达，发送SIGPROF

        - new_value: 设置定时器的属性
        
            struct itimerval {      // 定时器的结构体
            struct timeval it_interval;  // 每个阶段的时间，间隔时间
            struct timeval it_value;     // 延迟多长时间执行定时器
            };

            struct timeval {        // 时间的结构体
                time_t      tv_sec;     //  秒数     
                suseconds_t tv_usec;    //  微秒    
            };

        过10秒后，每个2秒定时一次
        
        - old_value ：记录上一次的定时的时间参数，一般不使用，指定NULL
    
    - 返回值：
        成功 0
        失败 -1 并设置错误号
*/

#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

//过3秒以后，每间隔2s定时一次
int main(){

    struct itimerval new_value;

    //设置间隔时间
    new_value.it_interval.tv_sec = 2;
    new_value.it_interval.tv_usec = 0;

    //设置延迟时间
    new_value.it_value.tv_sec = 3;
    new_value.it_value.tv_usec = 0; 
    
    int ret = setitimer(ITIMER_REAL,&new_value,NULL); //non-block 非阻塞
    printf("alarm is starting...\n");

    if(ret == -1){
        perror("setitimer");
        exit(0);
    }

    getchar();

    return 0;
}