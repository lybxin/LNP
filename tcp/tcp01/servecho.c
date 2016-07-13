#include<stdio.h>
#include<unistd.h>  //read write
#include<errno.h>   //error
#define MAXLINE 1024

void str_echo(int sockfd){
    ssize_t n;
    char buf[MAXLINE];

  again:
    while( (n = read(sockfd,buf,MAXLINE)) > 0){
        printf("server read %d bytes:%s",(int)n,buf);
        write(sockfd,buf,n);
    }

    if (n < 0 && errno == EINTR)
        goto again;
    else if (n < 0 ){
        printf("str_echo:read err %d, %d",(int)n,(int)errno);
        perror("servecho.c:");
    }

}
