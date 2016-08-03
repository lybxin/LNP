#include "../../common/common.h"

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr,Bindaddr;
    struct timespec now,res;
    char writebuf[TRANSSIZE];

    if (argc !=3){
        printf("argc error desip srcip\n");
        exit(1);
    }

    sockfd = Socket(AF_INET,SOCK_STREAM,0);

    memset(&Bindaddr,0,sizeof(Bindaddr));
    Bindaddr.sin_family = AF_INET;
    Bindaddr.sin_port =  htons(0);
    inet_pton(AF_INET,argv[2],&Bindaddr.sin_addr);
    errno = 0;
    Bind(sockfd,(SA*)&Bindaddr,sizeof(Bindaddr));
    printf("addr:%s\n",inet_ntoa(Bindaddr.sin_addr));
    perror("Bind");


    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET,argv[1],&servaddr.sin_addr);

    Connect(sockfd,(SA*)&servaddr,sizeof(servaddr));
    perror("Connect");
    
    snprintf(writebuf,TRANSSIZE,"hello");
    Write(sockfd,writebuf,strlen(writebuf)+1);
    perror("Write hello");

    sleep(20);
    snprintf(writebuf,TRANSSIZE,"world");
    Write(sockfd,writebuf,strlen(writebuf)+1);
    perror("Write world");

    clock_gettime(CLOCK_MONOTONIC,&now);
    res.tv_sec = now.tv_sec/10*10+10;
    res.tv_nsec = 0;
    printf("now sec:%ld  nsec:%ld res.sec%ld\n",now.tv_sec,now.tv_nsec,res.tv_sec);

    clock_nanosleep(CLOCK_MONOTONIC,TIMER_ABSTIME,&res,&now);
    Close(sockfd);

    return 0;

}
    















