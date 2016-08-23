
#include "../../common/common.h"
#include "../../common/rawops.h"
#include "../../common/tcp_private.h"

/*
void printinfo(struct tcp_info_user *info)
{
    printf("--------------------------------------------------------------------------\n");
    printf("tcpi_segs_in:%u,tcpi_segs_out:%u\n",info->tcpi_segs_in,info->tcpi_segs_out);
    printf("tcpi_unacked(packets_out):%u,tcpi_sacked:%u,tcpi_lost:%u,tcpi_fackets:%u,tcpi_retrans:%u\n",
            info->tcpi_unacked,info->tcpi_sacked,info->tcpi_lost,info->tcpi_fackets,info->tcpi_retrans);
    printf("tcpi_ca_state:%u,tcpi_rto:%u,tcpi_reordering:%u,rcv_nxt:%u,rcv_wup:%u,snd_nxt:%u,rcv_wnd:%u\n",
            info->tcpi_ca_state,info->tcpi_rto,info->tcpi_reordering,
            info->rcv_nxt,info->rcv_wup,info->snd_nxt,info->rcv_wnd);
    printf("gso_segs:%u,tcp_header_len:%u,pred_flags:%u,snd_una:%u,snd_wl1:%u,snd_wnd:%u,tcpi_snd_cwnd:%u\n",
            info->gso_segs,info->tcp_header_len,info->pred_flags,
            info->snd_una,info->snd_wl1,info->snd_wnd,info->tcpi_snd_cwnd);
    
}
*/

int main()
{
    int Listenfd,connfd;
    int len,val;
    int last_in = 0,last_out = 0,i = 0;
    struct tcp_info_user info;
    struct timespec;
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
    

