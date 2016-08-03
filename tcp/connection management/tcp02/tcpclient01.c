#include "../common/common.h"

#define SERV_PORT01 SERV_PORT
#define SERV_PORT02 10000

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in Bindaddr,servaddr;
    struct timespec now,res;

    if (argc !=2){
        printf("argc error");
        exit(1);
    }

    sockfd = Socket(AF_INET,SOCK_STREAM,0);

    memset(&Bindaddr,0,sizeof(Bindaddr));
    Bindaddr.sin_family = AF_INET;
    Bindaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    Bindaddr.sin_port = htons(SERV_PORT01);
    Bind(sockfd,(SA*)&Bindaddr,sizeof(Bindaddr));

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT02);
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
    















