#include "../../common/common.h"
#include "../../common/tcp_private.h"
#include "../../common/rawops.h"

void str_cli(FILE *fp,int sockfd);

int main(int argc, char **argv)
{
    int sockfd,i=0,last_in = 0,last_out = 0;
    struct sockaddr_in servaddr;
    int len,val;
    struct tcp_info_user info;
    char writebuf[TRANSSIZE];

    if (argc !=2){
        printf("argc error");
        exit(1);
    }

    sockfd = Socket(AF_INET,SOCK_STREAM,0);

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET,argv[1],&servaddr.sin_addr);

    Connect(sockfd,(SA*)&servaddr,sizeof(servaddr));

    ///*
    val = 1;
    len = sizeof(int);
    Setsockopt(sockfd, SOL_TCP, TCP_NODELAY,(void *)&val, len);
    //*/
    sleep_ms(3);
    for(i=0;i<20;i++)
    {
        snprintf(writebuf,TRANSSIZE,"world02");
        Write(sockfd,writebuf,58);
    }
    
    for(i=0;i<300;i++)
    {
        snprintf(writebuf,TRANSSIZE,"world02");
        Write(sockfd,writebuf,3);
        sleep_ms(3);
    }
    
    i = 0;
    while(i < 100*200)
    {
        len = sizeof(info);
        Getsockopt(sockfd, SOL_TCP, TCP_INFO,(void *)&info, (socklen_t *)&len);
        if( (last_in != info.tcpi_segs_in) || (last_out != info.tcpi_segs_out) )
        {
            //sleep_ms(1);
            len = sizeof(info);
            Getsockopt(sockfd, SOL_TCP, TCP_INFO,(void *)&info, (socklen_t *)&len);
            
            printftcpinfo(&info);

            printf("i=%d\n",i);
            
            
            last_in = info.tcpi_segs_in;
            last_out = info.tcpi_segs_out;
        }
     
        
        sleep_ms(1);
        i++;
    }
      
    sleep(100);    
    close(sockfd);
    
    return 0;

}
    















