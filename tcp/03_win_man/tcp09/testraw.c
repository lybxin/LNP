#include "../../common/common.h"
#include "../../common/rawops.h"
#include <pthread.h>  



void *recv_function(void *arg); 
void *send_function(void *arg); 



void *send_function(void *arg)
{
    int sockfd, tot_len, i=0;

    unsigned char buffer[MAX_PKT_SIZE] = {"welcome to linux hello\0"};  //MAX_PKT_SIZE
    
    sockfd = *( (int*)arg );
    
    
    senddelay = 20;
    
    tot_len = builddatapkt(buffer, recvacknumber,  1150);
    rawsend(sockfd, buffer, tot_len);
    
    //处理数据发送和业务逻辑
    senddelay = 0;
    for(i=0;i<29;i++)
    {   
        /*
        if(i == 10)
        {
            tcpwz = tcpwz*2;
            tot_len = builddatapkt(buffer, recvacknumber,  20);
            rawsend(sockfd, buffer, tot_len); 
            break;
        }
        */
        tot_len = builddatapkt(buffer, recvacknumber,  20);
        rawsend(sockfd, buffer, tot_len);  
    }
    
    senddelay = 20;
        

    

    
    sleep(300);
    
    return 0;
}


void *recv_function(void *arg)
{

    int sockfd, tot_len;
    u16 recvlen;
    unsigned char buffer[MAX_PKT_SIZE];
    
    //接收线程detach自己
    pthread_detach(pthread_self());
    
    sockfd = *( (int*)arg );

    while(1)
    {       
        //等待接收ACK
        recvlen = rawadvrecv(sockfd, buffer, MAX_PKT_SIZE,0);
        
        //判断是否收到了需要回复ACK的报文
        if(containdata(buffer, recvlen))
        {

            senddelay = 100;
            tot_len = buildackpkt(buffer,recvacknumber,TCP_TSOPT);
            //回复一个ACK报文 
            rawsend(sockfd,buffer,tot_len);

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
    senddelay = 700;
    
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


