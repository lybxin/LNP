#include<stdio.h>
#include<string.h>  //memset
#include<stdlib.h>  //exit
#include<unistd.h>  //fork close
//#include <netinet/in.h>   //addr_in?  yes
#include<arpa/inet.h>  //hton* ntoh*
#include<time.h>
#include<sys/types.h>
#include<sys/socket.h>

#define SERV_PORT 9877
#define LISTENQ 1024
#define SA struct sockaddr

void str_echo(int sockfd);

void main()
{
    int listenfd,connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    listenfd = socket(AF_INET,SOCK_STREAM,0);

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port =  htons(SERV_PORT);
    
    bind(listenfd,(SA*)&servaddr,sizeof(servaddr));

    listen(listenfd,LISTENQ);
    
    for( ; ;){
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd,(SA*)&cliaddr,&clilen);
        if( (childpid=fork()) == 0){
            close(listenfd);
            str_echo(connfd);
            printf("server read end\n");
            sleep(1);
            exit(0);
        }
        close(connfd);
    }
}
    

