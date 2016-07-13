#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<time.h>
#include<errno.h>

#define SERV_PORT01 9877
#define SERV_PORT02 10000
#define SA struct sockaddr

void str_cli(FILE *fp,int sockfd);

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;
    struct timespec now,res;

    if (argc !=2){
        printf("argc error");
        exit(1);
    }

    sockfd = socket(AF_INET,SOCK_STREAM,0);

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT01);
    inet_pton(AF_INET,argv[1],&servaddr.sin_addr);
    
    clock_gettime(CLOCK_MONOTONIC,&now);
    res.tv_sec = now.tv_sec/10*10+20;
    res.tv_nsec = 0;
    printf("now sec:%ld  nsec:%ld res.sec%ld\n",now.tv_sec,now.tv_nsec,res.tv_sec);

    clock_nanosleep(CLOCK_MONOTONIC,TIMER_ABSTIME,&res,&now);
    connect(sockfd,(SA*)&servaddr,sizeof(servaddr));
    
    //printf("now sec:%ld  nsec:%ld res.sec%ld",now.tv_sec,now.tv_nsec,res.tv_sec);
    perror("connect");
    
    sleep(10);

    close(sockfd);

    exit(0);

}
    















