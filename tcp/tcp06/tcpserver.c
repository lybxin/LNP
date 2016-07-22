#include<stdio.h>
#include<time.h>
#include<string.h>  //memset
#include<stdlib.h>  //exit
#include<unistd.h>  //fork close
//#include <netinet/in.h>   //addr_in?  yes
#include<arpa/inet.h>  //hton* ntoh*
#include<time.h>
#include<sys/types.h>
#include<sys/socket.h>
#include "common.h"

//void str_echo(int sockfd);

void main(int argc, char **argv)
{
    int listenfd,connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    struct timespec res,now;
    char readbuf[TRANSSIZE];

    listenfd = socket(AF_INET,SOCK_STREAM,0);

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    //servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port =  htons(SERV_PORT);
    inet_pton(AF_INET,argv[1],&servaddr.sin_addr);
    
    bind(listenfd,(SA*)&servaddr,sizeof(servaddr));

    listen(listenfd,LISTENQ);
    
    for( ; ;){
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd,(SA*)&cliaddr,&clilen);
        
        clock_gettime(CLOCK_MONOTONIC,&now);
        res.tv_sec = now.tv_sec/10*10+10;
        res.tv_nsec = 0;
        printf("now sec:%ld  nsec:%ld res.sec%ld\n",now.tv_sec,now.tv_nsec,res.tv_sec);
        
        read(connfd,readbuf,TRANSSIZE);
	printf("recv:%s\n",readbuf);

        read(connfd,readbuf,TRANSSIZE);
	printf("recv:%s\n",readbuf);

        clock_nanosleep(CLOCK_MONOTONIC,TIMER_ABSTIME,&res,&now);
        
        close(connfd);
    }
}
    

