#include "../../common/common.h"

int main(int argc, char **argv)
{
    int Listenfd,connfd;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    char readbuf[TRANSSIZE];

    Listenfd = Socket(AF_INET,SOCK_STREAM,0);

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port =  htons(SERV_PORT);
    //inet_pton(AF_INET,argv[1],&servaddr.sin_addr);
    
    Bind(Listenfd,(SA*)&servaddr,sizeof(servaddr));

    Listen(Listenfd,LISTENQ);
    
    for( ; ;){
        clilen = sizeof(cliaddr);
        connfd = Accept(Listenfd,(SA*)&cliaddr,&clilen);
        
        Read(connfd,readbuf,TRANSSIZE);
        printf("recv:%s\n",readbuf);
        
        Shutdown(connfd,SHUT_WR);
        
        Read(connfd,readbuf,TRANSSIZE);
        printf("recv:%s\n",readbuf);
        
        Close(connfd);
    }
    
    return 0;
}
    

