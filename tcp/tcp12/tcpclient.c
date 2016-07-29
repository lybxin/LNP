#include "common.h"


int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;
    struct timespec now,res;
    char writebuff[TRANSSIZE];

    if (argc !=2){
        printf("argc error");
        exit(1);
    }

    sockfd = socket(AF_INET,SOCK_STREAM,0);

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET,argv[1],&servaddr.sin_addr);

    Connect(sockfd,(SA*)&servaddr,sizeof(servaddr));
    
    write(sockfd,writebuff,10);

    sleep(1);


    Shutdown(sockfd, SHUT_WR);

    Write(sockfd,writebuff,TRANSSIZE);
    printf("half close write\n");

    Close(sockfd);


    exit(0);

}
    















