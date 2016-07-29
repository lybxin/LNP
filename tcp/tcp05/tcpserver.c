#include "../common/common.h"

int main()
{
    int Listenfd,connfd,ret;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    char readbuf[TRANSSIZE];
    int qlen = 5;

    Listenfd = Socket(AF_INET,SOCK_STREAM,0);

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port =  htons(SERV_PORT);
    
    Bind(Listenfd,(SA*)&servaddr,sizeof(servaddr));

    Listen(Listenfd,LISTENQ);

    ret = setsockopt(Listenfd, SOL_TCP, TCP_FASTOPEN, &qlen, sizeof(qlen)); 
    printf("server ret %d\n",ret); 
    if (ret < 0) {  
        printf ("setsockopt error: %s(errno: %d)\n", strerror (errno), errno);  
    } 

    for( ; ;){
        clilen = sizeof(cliaddr);
        connfd = Accept(Listenfd,(SA*)&cliaddr,&clilen);
        
        Read(connfd,readbuf,TRANSSIZE);
        
        Close(connfd);
    }
    
    return 0;
}
    

