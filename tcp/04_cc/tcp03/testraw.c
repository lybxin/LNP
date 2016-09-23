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
    u32 ackflag =TCP_TSOPT;
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
            
            if(i==2 || i==3)
            {
                continue;
            }
            //模拟延迟ABC  一个报文回复多个个ACK确认包
            if( i == 5  )
            {
                adddelaylinktail(50, recvacknumber-20, ackflag);
                sleep_ms(2);
                adddelaylinktail(50, recvacknumber-10, ackflag);
                sleep_ms(2);
                adddelaylinktail(50, recvacknumber, ackflag);
                sleep_ms(2);
                continue;
            }
            
            /* ack 压缩演示失败
            if(i==6 || i==7)
            {
                adddelaylinktail(50, recvacknumber, ackflag|TCP_USER1);
                continue;            
            }
            */
            
            adddelaylinktail(50, recvacknumber, ackflag);
            
        } 
    
    }

}


void *ack_function(void *arg)
{

    int sockfd;//i=0;
    struct timespec timeout;
    u32 acknumber, flag, tot_len;//ackcomplen[2];
    //unsigned char ackcompbuffer[2][MAX_PUREACK];
    unsigned char buffer[MAX_PKT_SIZE];
    
    //接收线程detach自己
    pthread_detach(pthread_self());
    
    sockfd = *( (int*)arg );

    while(popdelaylinkhead(&timeout, &acknumber, &flag, buffer, &tot_len))
    {       

        sleep_abs_ms(&timeout);
        
        /* ack 压缩
        if(flag & TCP_USER1)
        {
            memcpy(ackcompbuffer[i],buffer,100);
            ackcomplen[i]=tot_len;
            i++;
            printf("[ack_function] ack compress acknumber:%u \n",acknumber);
            continue;        
        }
        while(i>0)
        {
            rawsendnodelay(sockfd,ackcompbuffer[2-i],ackcomplen[2-i]);
            printf("[ack_function] ack compress ackcomplen:%u \n",ackcomplen[2-i]);
            --i;        
        }
        */
        
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


