
#include "../../common/common.h"
#include "../../common/rawops.h"
#include "../../common/tcp_private.h"


int main()
{
    int Listenfd,connfd;
    int len,val;
    int last_in = 0,last_out = 0,i = 0;
    struct tcp_info_user info;
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
    Getsockopt(Listenfd, SOL_TCP, TCP_USER_TIMEOUT,(void *)&val, (socklen_t *)&len);
	printf("user_timeout:%d\n",val);
    for( ; ;){
        clilen = sizeof(cliaddr);
        connfd = Accept(Listenfd,(SA*)&cliaddr,&clilen);
        
        sleep(3);
        
        val = 1;
        len = sizeof(int);
        Setsockopt(connfd, SOL_TCP, TCP_NODELAY,(void *)&val, len);
        
        
        snprintf(writebuf,TRANSSIZE,"world01");
        Write(connfd,writebuf,strlen(writebuf)+1);
        
        sleep_ms(10);
        snprintf(writebuf,TRANSSIZE,"world02");
        Write(connfd,writebuf,strlen(writebuf)+1);
        
        sleep_ms(10);
        snprintf(writebuf,TRANSSIZE,"world03");
        Write(connfd,writebuf,strlen(writebuf)+1);
        
        sleep_ms(10);
        snprintf(writebuf,TRANSSIZE,"world04");
        Write(connfd,writebuf,strlen(writebuf)+1);
        
        
        sleep_ms(10);
        snprintf(writebuf,TRANSSIZE,"world05");
        Write(connfd,writebuf,strlen(writebuf)+1);
        
        i = 0;
        while(i < 100*200)
        {
            len = sizeof(info);
            Getsockopt(connfd, SOL_TCP, TCP_INFO,(void *)&info, (socklen_t *)&len);
            if( (last_in != info.tcpi_segs_in) || (last_out != info.tcpi_segs_out) )
            {
                sleep_ms(2);
                len = sizeof(info);
                Getsockopt(connfd, SOL_TCP, TCP_INFO,(void *)&info, (socklen_t *)&len);
                
                printftcpinfo(&info);
                printf("i=%d\n",i);
                last_in = info.tcpi_segs_in;
                last_out = info.tcpi_segs_out;
            }
            
            sleep_ms(10);
            i++;
        }

        //printf("after write rto:%u, retrans:%u,rtt:%u,reorder:%u\n",info.tcpi_rto,info.tcpi_retransmits,info.tcpi_rtt,info.tcpi_reordering);
        
        
        
        //sleep(200);
 
        
        len = sizeof(info);
        Getsockopt(connfd, SOL_TCP, TCP_INFO,(void *)&info, (socklen_t *)&len);
        //printf("before close rto:%u, retrans:%u,rtt:%u,reorder:%u\n",info.tcpi_rto,info.tcpi_retransmits,info.tcpi_rtt,info.tcpi_reordering);
        
        printftcpinfo(&info);
        printf("close\n");
        Close(connfd);
    }
    return 0;
}
    

