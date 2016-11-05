#include "../../common/common.h"
#include "../../common/rawops.h"
#include <pthread.h>  



void *recv_function(void *arg); 
void *send_function(void *arg); 



void *send_function(void *arg)
{
    int sockfd, tot_len;
    //int i=0;
    unsigned char buffer[MAX_PKT_SIZE] = {"welcome to linux hello\0"};
    
    sockfd = *( (int*)arg );
    
    
    senddelay = 0;
    
    //处理数据发送和业务逻辑
    tot_len = builddatapkt(buffer, recvacknumber,  strlen((const char *)buffer));
    rawsend(sockfd, buffer, tot_len);
    sleep_ms(10);
    

    
    sleep(300);
    
    return 0;
}


void *recv_function(void *arg)
{

    int sockfd;
    u16 recvlen,i = 0;
    u32 rtoseq = 0, rtocnt = 0, ackflag =TCP_TSOPT;
    unsigned char buffer[MAX_PKT_SIZE];
    
    //接收线程detach自己
    pthread_detach(pthread_self());
    
    sockfd = *( (int*)arg );

    while(1)
    {       
        //等待接收ACK
        recvlen = rawadvrecv(sockfd, buffer, MAX_PKT_SIZE,TCP_SACKOPT);
        
        //判断是否收到了需要回复ACK的报文
        if(containdata(buffer, recvlen))
        {   
            i++;

            if(i < 8 )
            {
                adddelaylinktail(50, recvacknumber, ackflag);
                continue;
            }
            
            if(i==8)
            {
                rtoseq = recvseq;
                rtocnt = 3;
                continue;
            }
            /*原计划模拟第二次RTO超时
            if( i==(rto_i+7) && rto_i>0 )
            {
                rtoseq = recvseq;
                rtocnt = 2;
                continue;
            }
            */
            
            if(rtoseq == recvseq)
            {
                rtocnt = rtocnt>0?rtocnt-1:0;
                //rto_i = i;
                resetsackblk();
                del_ofo_link();
                ackflag |= TCP_SACKOPT;
                recvacknumber =recvendseq;
            }
            
            if(rtocnt == 0)
            {
                adddelaylinktail(50, recvacknumber,ackflag);
                continue;
            }
            
            

            

        } 
    
    }

}


void *ack_function(void *arg)
{

    int sockfd;
    struct timespec timeout;
    u32 acknumber, flag, tot_len;
    unsigned char buffer[MAX_PKT_SIZE];
    
    //接收线程detach自己
    pthread_detach(pthread_self());
    
    sockfd = *( (int*)arg );

    while(popdelaylinkhead(&timeout, &acknumber, &flag, buffer, &tot_len))
    {       
        sleep_abs_ms(&timeout);
        
        //tot_len = buildackpkt(buffer,acknumber,flag|TCP_TSOPT);
        
        //printf("-------tot_len:%u-----\n",tot_len);
        //回复一个ACK报文 
        rawsendnodelay(sockfd,buffer,tot_len);
    
    }
    
    return 0;
}

int main(int argc, char **argv)
{

    int sockfd;
    int res;  
    pthread_t recv_thread, send_thread, ack_thread;  
    void *thread_result;  
    
    strncpy(srcip, "127.0.0.2", 32);
    //延迟500ms发包 模拟500ms的rtt
    senddelay = 50;
    
    //connect前设置mss为200
    mssval = (50+12);

    sockfd = Socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
  
    
    initrawops(sockfd);
    printf("---------before conn setup-------------\n");
    rawconnect(sockfd);
    printf("---------conn setup-------------\n");
    //sleep(50);
    

    res = pthread_create(&recv_thread, NULL, recv_function, (void *)(&sockfd));  
    if (res != 0)  
    {  
        perror("Thread creation failed!");  
        exit(EXIT_FAILURE);  
    }  
    
    res = pthread_create(&ack_thread, NULL, ack_function, (void *)(&sockfd));  
    if (res != 0)  
    {  
        perror("Thread creation failed!");  
        exit(EXIT_FAILURE);  
    }  
    
    res = pthread_create(&send_thread, NULL, send_function, (void *)(&sockfd));  
    if (res != 0)  
    {  
        perror("Thread creation failed!");  
        exit(EXIT_FAILURE);  
    }  
    
    res = pthread_join(send_thread, &thread_result);  
    if (res != 0)  
    {  
        perror("Thread join failed!/n");  
        exit(EXIT_FAILURE);  
    }  
    
    
    rawconnrst(sockfd);
    
    //close之类的清理工作交给OS
    return 0;
}


