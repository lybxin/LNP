#include "common.h"


void main()
{
    int listenfd,connfd,ret;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    struct timespec res,now;
    char readbuf[TRANSSIZE];
    int qlen = 5;

    listenfd = socket(AF_INET,SOCK_STREAM,0);

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port =  htons(SERV_PORT);
    
    bind(listenfd,(SA*)&servaddr,sizeof(servaddr));

    listen(listenfd,LISTENQ);

    ret = setsockopt(listenfd, SOL_TCP, TCP_FASTOPEN, &qlen, sizeof(qlen)); 
    printf("server ret %d\n",ret); 
    if (ret < 0) {  
        printf ("setsockopt error: %s(errno: %d)\n", strerror (errno), errno);  
    } 

    for( ; ;){
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd,(SA*)&cliaddr,&clilen);
        
        read(connfd,readbuf,TRANSSIZE);
        
        close(connfd);
    }
}
    

