#include "../../common/common.h"
#include "../../common/rawops.h"
#include <pthread.h>  



void *recv_function(void *arg); 
void *send_function(void *arg); 



void *send_function(void *arg)
{
    int sockfd, tot_len;
    //u16 lostseq1;//,lostseq2,lostseq3,lostseq4;
    unsigned char buffer[MAX_PKT_SIZE] = {"hello\n\0"};
    
    sockfd = *( (int*)arg );
    
    //处理数据发送和业务逻辑
    tot_len = builddatapkt(buffer, recvacknumber,  strlen((const char *)buffer));
    rawsend(sockfd, buffer, tot_len);
    sleep(1);
    
    
    sleep(300);
    
    return 0;
}


void *recv_function(void *arg)
{

    int sockfd, tot_len, i=0;
    u16 recvlen;
    //u32 flag;
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
             
            if(i==0)
            {
                senddelay = 500;
                tot_len = buildackpkt(buffer,recvacknumber,TCP_TSOPT);
                //回复一个ACK报文 
                rawsend(sockfd,buffer,tot_len);
            
            }  
            
            /*
            if(i==4)
            {
                senddelay = 500;
                tot_len = buildackpkt(buffer,recvacknumber,TCP_TSOPT);
                //回复一个ACK报文 
                rawsend(sockfd,buffer,tot_len);
            
            } 
            */
            
            if(i==2 || i==3 )
            {
                senddelay = 20;
                recvacknumber = recvacknumber - (i) * 8;
                resetsackblk();
                appendsackblk((recvacknumber+8),(recvacknumber+(i)*8));
                tot_len = buildackpkt(buffer,recvacknumber,TCP_TSOPT|TCP_SACKOPT);
                //回复一个ACK报文 
                rawsend(sockfd,buffer,tot_len);
            
            }             
                      
            /*
            if(i > 7)
            {
                senddelay = 500;
                tot_len = buildackpkt(buffer,recvacknumber,TCP_TSOPT);
                //回复一个ACK报文 
                rawsend(sockfd,buffer,tot_len);
            }
            */

            i++;

        } 
    
    }

}


int main(int argc, char **argv)
{
    //int tot_len, recvlen, sockfd;
    int sockfd;
    //u32 lastacknumber;

    
    int res;  
    pthread_t recv_thread, send_thread;  
    void *thread_result;  
    
    //延迟500ms发包 模拟500ms的rtt
    senddelay = 500;

    sockfd = Socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
  
    
    initrawops(sockfd);
    rawconnect(sockfd);
    //sleep(500);
    

    res = pthread_create(&recv_thread, NULL, recv_function, (void *)(&sockfd));  
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


