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
    u16 recvlen,tmplen,i = 0;
    u32 ackflag =TCP_TSOPT;
    unsigned char buffer[MAX_PKT_SIZE];
    unsigned char tmpbuf[MAX_PKT_SIZE];
    
    //接收线程detach自己
    pthread_detach(pthread_self());
    
    sockfd = *( (int*)arg );

    while(1)
    {       
        printf("-----[recv_function] i:%d,recvacknumber:%u\n",i,recvacknumber);
        //等待接收ACK
        if(i==3)
        {
            tmplen = rawadvrecv(sockfd, tmpbuf, MAX_PKT_SIZE,TCP_SACKOPT|TCP_DISCARD);
            ++i;
            continue;
        }else if(i==4||i==5||i==7||i==12)
        {
            recvlen = rawadvrecv(sockfd, buffer, MAX_PKT_SIZE,TCP_SACKOPT|TCP_DISCARD);
            ++i;
            continue;
        }else
        {
            recvlen = rawadvrecv(sockfd, buffer, MAX_PKT_SIZE,TCP_SACKOPT);
        }
        
        
        //判断是否收到了需要回复ACK的报文
        if(containdata(buffer, recvlen))
        {   
            //i++;
            //adddelaylinktail(50, recvacknumber,ackflag|TCP_SACKOPT);
            
            if(i==6)
            {
                adddelaylinktail(48, recvacknumber,ackflag|TCP_SACKOPT);
                updaterecvstate(tmpbuf, tmplen,TCP_SACKOPT);
                adddelaylinktail(52, recvacknumber,ackflag|TCP_SACKOPT);
                i++;
                continue;
            }
            adddelaylinktail(50, recvacknumber,ackflag|TCP_SACKOPT);
            i++;

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


