
#include "../../common/common.h"
#include "../../common/rawops.h"
#include "../../common/tcp_private.h"



int main()
{
    int Listenfd,connfd;
    struct timespec;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    Listenfd = Socket(AF_INET,SOCK_STREAM,0);

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port =  htons(SERV_PORT);
    
    Bind(Listenfd,(SA*)&servaddr,sizeof(servaddr));
    Listen(Listenfd,LISTENQ);

    for( ; ;){
        clilen = sizeof(cliaddr);
        connfd = Accept(Listenfd,(SA*)&cliaddr,&clilen);
        
        sleep(200);
        
        printf("close\n");
        Close(connfd);
    }
    return 0;
}
    

