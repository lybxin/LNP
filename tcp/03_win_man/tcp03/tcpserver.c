
#include "../../common/common.h"
#include "../../common/rawops.h"
#include "../../common/tcp_private.h"



int main()
{
    int Listenfd,connfd;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    int len,i=0,last_in = 0,last_out = 0;
    struct tcp_info_user info;
    //char writebuf[TRANSSIZE];
    
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
        
        len = sizeof(info);
        Getsockopt(connfd, SOL_TCP, TCP_INFO,(void *)&info, (socklen_t *)&len);
        printftcpinfo(&info);
        
        
        
        
        i = 0;
        while(i < 100*200)
        {
            len = sizeof(info);
            Getsockopt(connfd, SOL_TCP, TCP_INFO,(void *)&info, (socklen_t *)&len);
            if( (last_in != info.tcpi_segs_in) || (last_out != info.tcpi_segs_out) )
            {
                //sleep_ms(1);
                len = sizeof(info);
                Getsockopt(connfd, SOL_TCP, TCP_INFO,(void *)&info, (socklen_t *)&len);
                
                printftcpinfo(&info);

                printf("i=%d\n",i);
                
                
                last_in = info.tcpi_segs_in;
                last_out = info.tcpi_segs_out;
            }
         
            
            sleep_ms(1);
            i++;
        }
            
        sleep(200);
        
        printf("close\n");
        Close(connfd);
    }
    
    
    return 0;
}
    

