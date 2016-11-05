#include "../../common/common.h"
#include "../../common/tcp_private.h"
#include "../../common/rawops.h"
#include <pthread.h> 

void *send_function(void *arg)
{
    int sockfd;

    char buffer[MAX_PKT_SIZE] = {"welcome to linux hello\0"};  //MAX_PKT_SIZE
    
    //接收线程detach自己
    pthread_detach(pthread_self());
    
    sockfd = *( (int*)arg );
    
    sleep_ms(3);
    Write(sockfd,buffer,100);
    

    
    return 0;
}

void *recv_function(void *arg)
{
    //int sockfd;

    //char buffer[MAX_PKT_SIZE] = {"welcome to linux hello\0"};  //MAX_PKT_SIZE
    
    //接收线程detach自己
    pthread_detach(pthread_self());
    
    //sockfd = *( (int*)arg );
    
    sleep_ms(30);
    //Read(sockfd,buffer,150);
    
    
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
            sleep_ms(3);
            len = sizeof(info);
            Getsockopt(sockfd, SOL_TCP, TCP_INFO,(void *)&info, (socklen_t *)&len);
            
            printftcpinfo(&info);

            printf("i=%d\n",i);
            
            
            last_in = info.tcpi_segs_in;
            last_out = info.tcpi_segs_out;
        }
     
        
        sleep_ms(3);
        i++;
    }  
    
    
    return 0;
}


int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr, bindaddr;
    int len,val=0;
    pthread_t recv_thread, send_thread,getinfo_thread; 
    int res;

    if (argc < 2){
        printf("argc error");
        exit(1);
    }
    if(argc >= 3)
    {
        val =atoi(argv[2]);
    }

    sockfd = Socket(AF_INET,SOCK_STREAM,0);
    
    //val = 1;
    len = sizeof(int);
    Setsockopt(sockfd, SOL_SOCKET, SO_DEBUG,(void *)&val, len);
    


    initskaddr(&servaddr,argv[1], SERV_PORT);
    Connect(sockfd,(SA*)&servaddr,sizeof(servaddr));

    sleep(10);
    
    shutdown(sockfd, SHUT_WR);
    
    sleep(10);
    /*
    res = pthread_create(&getinfo_thread, NULL, get_info, (void *)(&sockfd));  
    if (res != 0)  
    {  
        perror("Thread creation failed!");  
        exit(EXIT_FAILURE);  
    } 
          
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
    
    sleep(200);
    */
    //交由操作系统关闭文件描述符    
    //close(sockfd);
    
    return 0;

}
    















