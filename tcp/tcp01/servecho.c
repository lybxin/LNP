#include "../common/common.h"

#define MAXLINE 1024

void str_echo(int sockfd){
    ssize_t n;
    char buf[MAXLINE];

  again:
    while( (n = Read(sockfd,buf,MAXLINE)) > 0){
        printf("server Read %d bytes:%s",(int)n,buf);
        Write(sockfd,buf,n);
    }

    if (n < 0 && errno == EINTR)
        goto again;
    else if (n < 0 ){
        printf("str_echo:Read err %d, %d",(int)n,(int)errno);
        perror("servecho.c:");
    }

}
