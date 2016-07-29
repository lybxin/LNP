#include "common.h"


int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;
    char writebuf[TRANSSIZE];

    if (argc !=2){
        printf("argc error");
        exit(1);
    }

    snprintf(writebuf,TRANSSIZE,"hello");
    
    sockfd = Socket(AF_INET,SOCK_STREAM,0);

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET,argv[1],&servaddr.sin_addr);

    Connect(sockfd,(SA*)&servaddr,sizeof(servaddr));
    
    Write(sockfd,writebuf,strlen(writebuf)+1);

    sleep(1);

    Close(sockfd);

    printf("close write\n");
    Write(sockfd,writebuf,strlen(writebuf)+1);

    printf("close close\n");
    Close(sockfd);

    return 0;

}
    















