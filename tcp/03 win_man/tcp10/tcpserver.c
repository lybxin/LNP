
#include "../../common/common.h"
#include "../../common/rawops.h"
#include "../../common/tcp_private.h"
#include <pthread.h> 

void *send_function(void *arg)
{
    int sockfd;

    //char buffer[MAX_PKT_SIZE] = {"welcome to linux hello\0"};  //MAX_PKT_SIZE
    
    //接收线程detach自己
    pthread_detach(pthread_self());
    
    sockfd = *( (int*)arg );
    printf("[send_function]sockfd:%d \n",sockfd);
    
    sleep_ms(1500*1000);
    //Write(sockfd,buffer,50);    
    
    return 0;
}

void *recv_function(void *arg)
{
    int sockfd,len,i=0;
    struct tcp_info_user info;
    char buffer[MAX_PKT_SIZE] = {"welcome to linux hello\0"};  //MAX_PKT_SIZE
    
    //接收线程detach自己
    pthread_detach(pthread_self());
        
    sockfd = *( (int*)arg );
    //sleep_ms(15*1000);
    len = sizeof(info);
    Getsockopt(sockfd, SOL_TCP, TCP_INFO,(void *)&info, (socklen_t *)&len);
    printf("----------------start read--------------\n");
    printftcpinfo(&info);
    while(i < 200)
    {
        Read(sockfd,buffer,MAX_PKT_SIZE);
        //sleep_ms(2*1000);
        //len = sizeof(info);
        //Getsockopt(sockfd, SOL_TCP, TCP_INFO,(void *)&info, (socklen_t *)&len);
        //printf("\n----------read %d bytes--------------\n",(i+1)*num);
        //printftcpinfo(&info);
        i++;
    }
    
    
    
    return 0;
}

void *get_info(void *arg)
{
    int sockfd;
    int len,i=0,last_in = 0,last_out = 0;;
    struct tcp_info_user info;
    
    //接收线程detach自己
    pthread_detach(pthread_self());  
      
    sockfd = *( (int*)arg );
    
    
            
    i = 0;
    while(i < 100*200000)
    {
        len = sizeof(info);
        Getsockopt(sockfd, SOL_TCP, TCP_INFO,(void *)&info, (socklen_t *)&len);
        if( (last_in != info.tcpi_segs_in) || (last_out != info.tcpi_segs_out) )
        {
            sleep_ms(2);
            //len = sizeof(info);
            //Getsockopt(sockfd, SOL_TCP, TCP_INFO,(void *)&info, (socklen_t *)&len);
            
            printftcpinfo(&info);

            printf("i=%d\n",i);
            
            
            last_in = info.tcpi_segs_in;
            last_out = info.tcpi_segs_out;
        }
     
        
        sleep_ms(1);
        i++;
    }  
    
    return 0;
}

int main()
{
    int Listenfd,connfd;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    int val,len;
    pthread_t recv_thread, send_thread,getinfo_thread; 
    int res;

    Listenfd = Socket(AF_INET,SOCK_STREAM,0);

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port =  htons(SERV_PORT);
    
    Bind(Listenfd,(SA*)&servaddr,sizeof(servaddr));
    Listen(Listenfd,LISTENQ);
    

    for( ; ;){
    
        
        val = 1;
        len = sizeof(int);
        Setsockopt(Listenfd, SOL_SOCKET, SO_DEBUG,(void *)&val, len);
             
        clilen = sizeof(cliaddr);
        connfd = Accept(Listenfd,(SA*)&cliaddr,&clilen);
            
        val = 1;
        len = sizeof(int);
        Setsockopt(connfd, SOL_SOCKET, SO_DEBUG,(void *)&val, len);
        
        res = pthread_create(&getinfo_thread, NULL, get_info, (void *)(&connfd));  
        if (res != 0)  
        {  
            perror("Thread creation failed!");  
            exit(EXIT_FAILURE);  
        } 
                  
        res = pthread_create(&recv_thread, NULL, recv_function, (void *)(&connfd));  
        if (res != 0)  
        {  
            perror("Thread creation failed!");  
            exit(EXIT_FAILURE);  
        }  
        
        res = pthread_create(&send_thread, NULL, send_function, (void *)(&connfd));  
        if (res != 0)  
        {  
            perror("Thread creation failed!");  
            exit(EXIT_FAILURE);  
        }  


        
        //sleep(200);
        
        printf("close\n");
        //交由系统回收关闭文件描述符
        //Close(connfd);
    }
    return 0;
}
    

