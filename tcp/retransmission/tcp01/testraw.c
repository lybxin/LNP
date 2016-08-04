#include "../../common/common.h"
#include "../../common/rawops.h"
#include <pthread.h>  

void *recv_function(void *arg); 
void *send_function(void *arg); 


void *recv_function(void *arg)
{

    u32 lastacknumber;
    int sockfd, tot_len;
    unsigned char buffer[MAX_PKT_SIZE];
    
    sockfd = *( (int*)arg );
    //负责处理接收及ack回复工作
    lastacknumber = recvacknumber;
    while(1)
    {       
        //等待接收ACK
        rawrecv(sockfd, buffer, MAX_PKT_SIZE);
        
        if(lastacknumber != recvacknumber)
        {
            //acknumber发生变化  接收到了data 发送ACK
            tot_len = buildackpkt(buffer,recvacknumber,TCP_TSOPT);
            
            rawsend(sockfd,buffer,tot_len);
            lastacknumber = recvacknumber;
        }
    
    }

}


void *send_function(void *arg)
{
    int sockfd, tot_len;
    unsigned char buffer[MAX_PKT_SIZE] = {"hello\n\0"};
    
    sockfd = *( (int*)arg );
    
    //处理数据发送和业务逻辑
    tot_len = builddatapkt(buffer, recvacknumber,  strlen((const char *)buffer));
    rawsend(sockfd, buffer, tot_len);
    //sendflag = 1;
    
    return 0;
}


int main(int argc, char **argv)
{
    //int tot_len, recvlen, sockfd;
    int sockfd;
    //u32 lastacknumber;

    
    int res;  
    pthread_t recv_thread, send_thread;  
    void *thread_result;  

    sockfd = Socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
  
    
    initrawops(sockfd);
    rawconnect(sockfd);
    sleep(5);
    

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
    
    sleep(5);
    
    rawconnrst(sockfd);
    
    //close之类的清理工作交给OS
    return 0;
}


