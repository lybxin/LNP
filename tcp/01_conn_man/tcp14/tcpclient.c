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
    int len,val;
    pthread_t recv_thread, send_thread,getinfo_thread; 
    int res;
    char *ptr = "127.0.0.1";
    u16 port = 12345;

    if (argc < 2){
        printf("argc error");
        exit(1);
    }
    if(argc >= 3)
    {
        ptr = argv[2];
    }
    
    if(argc >= 4)
    {
        port = (u16)atoi(argv[3]);
    }

    sockfd = Socket(AF_INET,SOCK_STREAM,0);
    
    val = 1;
    len = sizeof(int);
    Setsockopt(sockfd, SOL_SOCKET, SO_DEBUG,(void *)&val, len);
    
    val = 1;
    len = sizeof(int);
    Setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,(void *)&val, len);
    
    
    val = 1;
    len = sizeof(int);
    Setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT,(void *)&val, len);
    
    initskaddr(&bindaddr, ptr, port);
    Bind(sockfd,(SA*)&bindaddr,sizeof(bindaddr));
    sleep_ms(10);
    
    //initskaddr(&servaddr,"127.0.0.4", 9877);
    //Connect(sockfd,(SA*)&servaddr,sizeof(servaddr));
    //printf("connect\n");
    sleep_ms(10);
    Listen(sockfd,10);
    printf("listen\n");
    
    int connfd = Accept(sockfd,NULL,NULL);
    
    if(fork() == 0)
    {
    
    Close(connfd);
    printf("14 child accept\n");
   
    }else
    {
    sleep(10);
    Close(connfd);
    printf("14 parent accept\n");
    
    }
    
    
    /*
    initskaddr(&bindaddr, "127.0.0.2", 0);
    Bind(sockfd,(SA*)&bindaddr,sizeof(bindaddr));
    sleep_ms(10);
    
    initskaddr(&bindaddr, "127.0.0.3", 0);
    Bind(sockfd,(SA*)&bindaddr,sizeof(bindaddr));
    sleep_ms(10);
    
    initskaddr(&bindaddr, "127.0.0.2", 10000);
    Bind(sockfd,(SA*)&bindaddr,sizeof(bindaddr));
    sleep_ms(10);
    
    initskaddr(&bindaddr, "127.0.0.2", 10001);
    Bind(sockfd,(SA*)&bindaddr,sizeof(bindaddr));
    sleep_ms(10);
    */

    initskaddr(&servaddr,argv[1], SERV_PORT);
    //Connect(sockfd,(SA*)&servaddr,sizeof(servaddr));

    sleep(1000);
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
    















