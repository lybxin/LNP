
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
                                
                if((last_in != info.tcpi_segs_in) && (last_out != info.tcpi_segs_out))
                {
                    //收到dup ACK并触发了快速重传  尝试写入新数据
                    Write(connfd,writebuf,strlen(writebuf)+1);
                    printf("----write new data-----\n");
                }
                
                printf("i=%d\n",i);
                
                last_in = info.tcpi_segs_in;
                last_out = info.tcpi_segs_out;
            }
            
            sleep_ms(10);
            i++;
        }

        
        //sleep(200);
 
        
        len = sizeof(info);
        Getsockopt(connfd, SOL_TCP, TCP_INFO,(void *)&info, (socklen_t *)&len);
        printftcpinfo(&info);
        
        printf("close\n");
        Close(connfd);
    }
    return 0;
}
    

