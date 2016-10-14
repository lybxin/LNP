#include "../../common/common.h"
#include "../../common/rawops.h"
#include <pthread.h>  



void *recv_function(void *arg); 
void *send_function(void *arg); 

//int dropno=-1;
int dropno=7;

void *send_function(void *arg)
{
/*
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
*/    
    return 0;
}


void *recv_function(void *arg)
{

    int sockfd;
    u16 recvlen,i = 0;
    u8 thflag=0;
    u32 ackflag = TCP_TSOPT|TCP_SACKOPT;
    unsigned char buffer[MAX_PKT_SIZE];
    
    //接收线程detach自己
    //pthread_detach(pthread_self());
    
    sockfd = *( (int*)arg );

    while(1)
    {   
    
        if(i==dropno)
        {
            rawadvrecv(sockfd, buffer, MAX_PKT_SIZE,TCP_SACKOPT|TCP_DISCARD);
            i++;
            continue;
        }    
        //等待接收ACK
        recvlen = rawadvrecv(sockfd, buffer, MAX_PKT_SIZE,TCP_SACKOPT);
        
        if(i==5)
        {
            ackflag |= TCP_ECE;
        }
        
        thflag = getthflag(buffer);
        
        if(thflag&TH_CWR)
        {
            ackflag &= ~TCP_ECE;
        }
        
        
        //判断是否收到了需要回复ACK的报文
        if(containdata(buffer, recvlen))
        {   
            adddelaylinktail(50, recvacknumber,ackflag);
        } 
        
        
        
        //判断是否收到了FIN
        if(thflag&TH_FIN)
        {   
            
            adddelaylinktail(50, recvacknumber,ackflag|TCP_SACKOPT|TCP_FIN);
            break;

        } 
        i++;
    
    }
    
    return 0;
}


void *ack_function(void *arg)
{

    int sockfd;
    struct timespec timeout;
    u32 acknumber, flag, tot_len;
    unsigned char buffer[MAX_PKT_SIZE];
    
    //接收线程detach自己
    //pthread_detach(pthread_self());
    
    sockfd = *( (int*)arg );

    while(popdelaylinkhead(&timeout, &acknumber, &flag, buffer, &tot_len))
    {       
        sleep_abs_ms(&timeout);
        
        //tot_len = buildackpkt(buffer,acknumber,flag|TCP_TSOPT);
        
        printf("[ack_function]-------acknumber:%u,flag:%u-----\n",acknumber,flag);
        //回复一个ACK报文 
        rawsendnodelay(sockfd,buffer,tot_len);
        if(flag&TCP_FIN)
        {
            break;
        }
    
    }
    
    printf("[ack_function]end-------acknumber:%u,flag:%u-----\n",acknumber,flag);
    
    return 0;
}

int main(int argc, char **argv)
{

    int sockfd;
    int res;  
    pthread_t recv_thread, send_thread, ack_thread;  
    void *thread_result;  
    
    if(argc==2)
    {
        dropno = atoi(argv[1]);
    }
        
    strncpy(srcip, "127.0.0.2", 32);
    //延迟500ms发包 模拟500ms的rtt
    senddelay = 50;
    
    //connect前设置mss为200
    mssval = (50+12);
    
    //关闭TSopt
    //tcptsopt = 0;

    sockfd = Socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
  
    initrawops(sockfd);
    printf("---------before conn setup-------------\n");
    //rawconnect(sockfd);
    rawadvconnect(sockfd, TCP_TSOPT|TCP_ECE|TCP_CWR, TCP_TSOPT);
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
    
    res = pthread_join(ack_thread, &thread_result);  
    if (res != 0)  
    {  
        perror("Thread join failed!/n");  
        exit(EXIT_FAILURE);  
    } 
    
    res = pthread_join(recv_thread, &thread_result);  
    if (res != 0)  
    {  
        perror("Thread join failed!/n");  
        exit(EXIT_FAILURE);  
    } 
    
    //rawconnrst(sockfd);
    
    //close之类的清理工作交给OS
    return 0;
}


