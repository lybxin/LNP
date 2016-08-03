#include "../common/common.h"

int main()
{
    int Listenfd,connfd;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    char writebuf[TRANSSIZE];

    snprintf(writebuf,TRANSSIZE,"world");
    
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
        

        sleep(3);
        printf("client closed server write begin\n");
        Write(connfd,writebuf,strlen(writebuf)+1);
        printf("client closed server write end\n");
        
        sleep(5);
        
        Close(connfd);
    }
    
    return 0;
}
    

