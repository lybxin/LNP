
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
    
    sleep_ms(10);
    
    sockfd = *( (int*)arg );
    
    printf("\n[send_function]----------sockfd %d--------------\n",sockfd);
    
    while(1){
    Accept(sockfd,NULL,NULL);
    printf("send end accept\n");
    }
    
   // Write(sockfd,buffer,200);
    
    printf("start close \n");    
    Close(sockfd);
    printf("start close \n");
    
    return 0;
}

void *recv_function(void *arg)
{
    int sockfd;
    

    //char buffer[MAX_PKT_SIZE] = {"welcome to linux hello\0"};  //MAX_PKT_SIZE
    
    //接收线程detach自己
    pthread_detach(pthread_self());
    
    printf("\n[recv_function]----------sockfd %d--------------\n",sockfd);
        
    sockfd = *( (int*)arg );
    
    printf("start accept \n");
    while(1){
    Accept(sockfd,NULL,NULL);
    printf("end accept \n");
    }
    sleep_ms(30);
    //num = Read(sockfd,buffer,MAX_PKT_SIZE);
    
    
    return 0;
}

void *get_info(void *arg)
{
    int sockfd;
    int len,i=0,last_in = 0,last_out = 0,ret=0;
    struct tcp_info_user info;
    
    //接收线程detach自己
    pthread_detach(pthread_self());  
      
    sockfd = *( (int*)arg );
    
    
            
    i = 0;
    while(i < 100*200000)
    {
        len = sizeof(info);
        ret = Getsockopt(sockfd, SOL_TCP, TCP_INFO,(void *)&info, (socklen_t *)&len);
        if(ret != 0)
        {
            break;
        }
        if( (last_in != info.tcpi_segs_in) || (last_out != info.tcpi_segs_out) )
        {       
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
    
    val = 1;
    len = sizeof(int);
    Setsockopt(Listenfd, SOL_SOCKET, SO_DEBUG,(void *)&val, len);
    
    val = 1;
    len = sizeof(int);
    Setsockopt(Listenfd, SOL_SOCKET, SO_REUSEPORT,(void *)&val, len);
        
    initskaddr(&servaddr, "0.0.0.0", SERV_PORT);
    Bind(Listenfd,(SA*)&servaddr,sizeof(servaddr));
    
    Listen(Listenfd,LISTENQ);
    

    for( ; ;){  
        
        /*
        val = 1;
        len = sizeof(int);
        Setsockopt(connfd, SOL_SOCKET, SO_DEBUG,(void *)&val, len);
        */

             
        clilen = sizeof(cliaddr);
        //connfd = Accept(Listenfd,(SA*)&cliaddr,&clilen);
            

        
        res = pthread_create(&getinfo_thread, NULL, get_info, (void *)(&Listenfd));  
        if (res != 0)  
        {  
            perror("Thread creation failed!");  
            exit(EXIT_FAILURE);  
        } 
                  
        res = pthread_create(&recv_thread, NULL, recv_function, (void *)(&Listenfd));  
        if (res != 0)  
        {  
            perror("Thread creation failed!");  
            exit(EXIT_FAILURE);  
        }  
        
        res = pthread_create(&send_thread, NULL, send_function, (void *)(&Listenfd));  
        if (res != 0)  
        {  
            perror("Thread creation failed!");  
            exit(EXIT_FAILURE);  
        }  


        
        sleep(1000);
        
        printf("close\n");
        //交由系统回收关闭文件描述符
        //Close(connfd);
    }
    return 0;
}
    

