#include "../../common/common.h"
#include "../../common/rawops.h"
#include <pthread.h>  

void *recv_function(void *arg); 
void *send_function(void *arg); 



void *send_function(void *arg)
{
    int sockfd, tot_len;
    unsigned char buffer[MAX_PKT_SIZE] = {"hello\n\0"};
    
    sockfd = *( (int*)arg );
    
    //处理数据发送和业务逻辑
    tot_len = builddatapkt(buffer, recvacknumber,  strlen((const char *)buffer));
    rawsend(sockfd, buffer, tot_len);
    //sendflag = 1;
    
    sleep(250);
    
    return 0;
}


void *recv_function(void *arg)
{

    //u32 lastacknumber;
    u16 recvlen;
    int sockfd, tot_len,i = 0;
    unsigned char buffer[MAX_PKT_SIZE];
    
    //接收线程detach自己
    pthread_detach(pthread_self());
    
    sockfd = *( (int*)arg );
    //负责处理接收及ack回复工作
    //lastacknumber = recvacknumber;
    while(1)
    {       
        //等待接收ACK
        recvlen = rawrecv(sockfd, buffer, MAX_PKT_SIZE);
        
        //判断是否收到了包含数据的报文
        if( containdata(buffer,recvlen) )
        {
            //构造dup ack 第一个数据包的ACK不回复 因此判断i>0
            if(i > 0 && i < 5)
            {
                recvacknumber = recvacknumber - (i+1) * 8;
                
                if(i == 1)
                {
                    senddelay = 500;
                }else
                {
                    senddelay = 20;
                }          
                
                //acknumber发生变化  接收到了data 发送ACK
                tot_len = buildackpkt(buffer,recvacknumber,TCP_TSOPT);
                //回复一个ACK报文 
                rawsend(sockfd,buffer,tot_len);
                
            }

            if(i > 7)
            {
                senddelay = 500;
                tot_len = buildackpkt(buffer,recvacknumber,TCP_TSOPT);
                //回复一个ACK报文 
                rawsend(sockfd,buffer,tot_len);
            }
            //lastacknumber = recvacknumber;
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
    //sleep(5);
    

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


