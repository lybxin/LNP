#include<stdio.h>
#include<time.h>
#include<string.h>  //memset
#include<stdlib.h>  //exit
#include<unistd.h>  //fork Close
//#include <netinet/in.h>   //addr_in?  yes
#include<arpa/inet.h>  //hton* ntoh*
#include<time.h>
#include<sys/types.h>
#include<sys/Socket.h>
#include "common.h"

//void str_echo(int sockfd);

void main(int argc, char **argv)
{
    int Listenfd,connfd,conut;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    struct timespec res,now;
    char readbuf[TRANSSIZE];
    int backlog = LISTENQ;
    
    printf("one\n");
    if(argc == 2){
        backlog = atoi(argv[1]);
     }

    Listenfd = Socket(AF_INET,SOCK_STREAM,0);

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port =  htons(SERV_PORT);
    
    Bind(Listenfd,(SA*)&servaddr,sizeof(servaddr));

    printf("backlog:%d\n",backlog);
    Listen(Listenfd,backlog);
    
    sleep(10000000);
    printf("end sleep");

    for( ; ;){
        clilen = sizeof(cliaddr);
        connfd = Accept(Listenfd,(SA*)&cliaddr,&clilen);
        
        clock_gettime(CLOCK_MONOTONIC,&now);
        res.tv_sec = now.tv_sec/10*10+10;
        res.tv_nsec = 0;
        printf("now sec:%ld  nsec:%ld res.sec%ld\n",now.tv_sec,now.tv_nsec,res.tv_sec);
        
        Read(connfd,readbuf,TRANSSIZE);
        clock_nanosleep(CLOCK_MONOTONIC,TIMER_ABSTIME,&res,&now);
        
        Close(connfd);
    }
}
    

