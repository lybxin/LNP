#include "../../common/common.h"

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;
    char writebuf[TRANSSIZE];   
    int mssopt, wsopt;
    
    if (argc !=4){
        printf("argc error:ip mssopt wsopt\n");
        exit(1);
    }
    
    mssopt = atoi(argv[2]);
    wsopt = atoi(argv[3]);
    snprintf(writebuf,TRANSSIZE,"hello");
    
    sockfd = socket(AF_INET,SOCK_STREAM,0);

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET,argv[1],&servaddr.sin_addr);
    
    Setsockopt(sockfd, SOL_TCP, TCP_MAXSEG,&mssopt, sizeof mssopt);
        
    Setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF,&wsopt, sizeof wsopt);

    Connect(sockfd,(SA*)&servaddr,sizeof(servaddr));

    Write(sockfd,writebuf,strlen(writebuf)+1);

    sleep(10);

    Close(sockfd);


    return 0;

}
    















