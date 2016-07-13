#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<arpa/inet.h>

#define SERV_PORT 9877
#define SA struct sockaddr

void str_cli(FILE *fp,int sockfd);

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;

    if (argc !=2){
        printf("argc error");
        exit(1);
    }

    sockfd = socket(AF_INET,SOCK_STREAM,0);

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET,argv[1],&servaddr.sin_addr);

    connect(sockfd,(SA*)&servaddr,sizeof(servaddr));

    str_cli(stdin,sockfd);

    exit(0);

}
    















