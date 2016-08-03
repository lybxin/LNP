#include "../../common/common.h"

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;
    char writebuf[TRANSSIZE],readbuf[TRANSSIZE];   
    
    if (argc !=2){
        printf("argc error\n");
        exit(1);
    }
    snprintf(writebuf,TRANSSIZE,"hello");
    
    sockfd = socket(AF_INET,SOCK_STREAM,0);

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET,argv[1],&servaddr.sin_addr);

    Connect(sockfd,(SA*)&servaddr,sizeof(servaddr));

    Write(sockfd,writebuf,strlen(writebuf)+1);

    sleep(1);

    Shutdown(sockfd, SHUT_WR);
    
    Read(sockfd, readbuf, TRANSSIZE);
    printf("%s\n",readbuf);
    
    //半关后write会收到sigpipe而提前终止程序
    Write(sockfd,writebuf,strlen(writebuf)+1);
    printf("half close Write\n");

    Close(sockfd);


    return 0;

}
    















