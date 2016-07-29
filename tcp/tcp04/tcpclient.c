#include "../common/common.h"

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;
    char writebuf[TRANSSIZE];

    if (argc != 2){
        printf("argc error: desip srcip\n");
        exit(1);
    }

    sockfd = Socket(AF_INET,SOCK_STREAM,0);

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET,argv[1],&servaddr.sin_addr);

    Connect(sockfd,(SA*)&servaddr,sizeof(servaddr));

    
    snprintf(writebuf,TRANSSIZE,"hello");
    Write(sockfd,writebuf,strlen(writebuf)+1);
    perror("Write hello");

    Shutdown(sockfd,SHUT_RD);
    
    sleep(20);
    
    snprintf(writebuf,TRANSSIZE,"world");
    Write(sockfd,writebuf,strlen(writebuf)+1);
    perror("Write world");


    Close(sockfd);

    return 0;

}
    















