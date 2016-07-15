#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<time.h>
#include "common.h"

//void str_cli(FILE *fp,int sockfd);

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;
    struct timespec now,res;
    char writebuf[TRANSSIZE];

    if (argc !=2){
        printf("argc error");
        exit(1);
    }

    sockfd = socket(AF_INET,SOCK_STREAM,0);

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET,argv[1],&servaddr.sin_addr);

    connect(sockfd,(SA*)&servaddr,sizeof(servaddr));
    
    write(sockfd,writebuf,TRANSSIZE);

    clock_gettime(CLOCK_MONOTONIC,&now);
    res.tv_sec = now.tv_sec/10*10+10;
    res.tv_nsec = 0;
    printf("now sec:%ld  nsec:%ld res.sec%ld\n",now.tv_sec,now.tv_nsec,res.tv_sec);

    clock_nanosleep(CLOCK_MONOTONIC,TIMER_ABSTIME,&res,&now);
    close(sockfd);

    exit(0);

}
    














