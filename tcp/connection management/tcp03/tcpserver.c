#include "../common/common.h"

int main()
{
    int Listenfd,connfd;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    struct timespec res,now;
    char readbuf[TRANSSIZE];

    Listenfd = Socket(AF_INET,SOCK_STREAM,0);

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port =  htons(SERV_PORT);
    
    Bind(Listenfd,(SA*)&servaddr,sizeof(servaddr));

    Listen(Listenfd,LISTENQ);
    
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
    
    return 0;
}
    

