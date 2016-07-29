#include "common.h"


void main()
{
    int listenfd,connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    struct timespec res,now;
    char writebuff[TRANSSIZE];

    listenfd = Socket(AF_INET,SOCK_STREAM,0);

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port =  htons(SERV_PORT);
    
    Bind(listenfd,(SA*)&servaddr,sizeof(servaddr));

    Listen(listenfd,LISTENQ);
    
    for( ; ;){
        clilen = sizeof(cliaddr);
        connfd = Accept(listenfd,(SA*)&cliaddr,&clilen);
        
        sleep(3);
        Write(connfd,writebuff,TRANSSIZE);

        sleep(5);
        close(connfd);
    }
}
    

