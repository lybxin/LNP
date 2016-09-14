#include "../../common/common.h"

#define SERV_PORT01 SERV_PORT
#define SERV_PORT02 10000

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in bindaddr,servaddr;
    struct timespec now,res;

    if (argc !=2){
        printf("argc error");
        exit(1);
    }

    sockfd = Socket(AF_INET,SOCK_STREAM,0);

    memset(&bindaddr,0,sizeof(bindaddr));
    bindaddr.sin_family = AF_INET;
    bindaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bindaddr.sin_port = htons(SERV_PORT02);
    //绑定本地地址
    Bind(sockfd,(SA*)&bindaddr,sizeof(bindaddr));

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT01);
    inet_pton(AF_INET,argv[1],&servaddr.sin_addr);
    
    clock_gettime(CLOCK_MONOTONIC,&now);
    res.tv_sec = now.tv_sec/10*10+10;
    res.tv_nsec = 0;
    printf("now sec:%ld  nsec:%ld res.sec%ld\n",now.tv_sec,now.tv_nsec,res.tv_sec);

    clock_nanosleep(CLOCK_MONOTONIC,TIMER_ABSTIME,&res,&now);
    Connect(sockfd,(SA*)&servaddr,sizeof(servaddr));
    
    printf("Connect\n");
    
    sleep(10);

    Close(sockfd);

    return 0;

}
    















