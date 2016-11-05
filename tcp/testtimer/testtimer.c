#include<stdio.h>
#include<time.h>

void main(){
    struct timespec res;

    clock_getres(CLOCK_REALTIME,&res);
    printf("realtime %ld,%ld",res.tv_sec,res.tv_nsec);

    clock_getres(CLOCK_MONOTONIC,&res);
    printf("mono %ld,%ld",res.tv_sec,res.tv_nsec);

    clock_getres(CLOCK_PROCESS_CPUTIME_ID,&res);
    printf("cputime %ld,%ld",res.tv_sec,res.tv_nsec);

}
