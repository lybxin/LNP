#include "../../common/common.h"

int main()
{
    int Listenfd,connfd;
    int user_timeout,err,len,lastrto=0,i=0;
    struct tcp_info info;
    struct timespec req,remain;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    char writebuf[TRANSSIZE];
    
    snprintf(writebuf,TRANSSIZE,"hello");

    Listenfd = Socket(AF_INET,SOCK_STREAM,0);

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port =  htons(SERV_PORT);
    
    Bind(Listenfd,(SA*)&servaddr,sizeof(servaddr));

    Listen(Listenfd,LISTENQ);
    
    len = sizeof(int);
    Getsockopt(Listenfd, SOL_TCP, TCP_USER_TIMEOUT,(void *)&user_timeout, (socklen_t *)&len);
	printf("user timeout:%d\n",user_timeout);
    for( ; ;){
        clilen = sizeof(cliaddr);
        connfd = Accept(Listenfd,(SA*)&cliaddr,&clilen);
        
        sleep(3);
        len = sizeof(info);
        Getsockopt(connfd, SOL_TCP, TCP_INFO,(void *)&info, (socklen_t *)&len);
        printf("before write rto:%u, retrans:%u,rtt:%u\n",info.tcpi_rto,info.tcpi_retransmits,info.tcpi_rtt);
        
        Write(connfd,writebuf,strlen(writebuf)+1);
        
        snprintf(writebuf,TRANSSIZE,"world");
        Write(connfd,writebuf,strlen(writebuf)+1);
        printf("serv write hello  and  world i:%d\n",i);;
        
        i = 0;
        while(i < 1300)
        {
            len = sizeof(info);
            Getsockopt(connfd, SOL_TCP, TCP_INFO,(void *)&info, (socklen_t *)&len);
            if(lastrto != info.tcpi_rto)
            {
                lastrto = info.tcpi_rto;
                printf("i:%d,rto:%u , retrans:%u,rtt:%u\n",i,info.tcpi_rto,info.tcpi_retransmits,info.tcpi_rtt);
            }
            
            req.tv_sec = 0;
    	    req.tv_nsec = 200000000;
    	    clock_nanosleep(CLOCK_MONOTONIC,0,&req,&remain);
    	    i++;
        }


        //sleep(50);
        len = sizeof(int);
        Getsockopt(connfd, SOL_SOCKET, SO_ERROR, (void *)&err, (socklen_t *)&len);
        if(err > 0)
        {
            printf("%s\n",strerror(err));
        }
        printf("before close\n");
        Close(connfd);
    }
    return 0;
}
    

