
#include "../../common/common.h"
#include "../../common/rawops.h"
#include "../../common/tcp_private.h"
#include <pthread.h> 
//该flag有多线程竞争关系 需要同步  此处影响不大 忽略
int openflag = 1;
int pktnum = 5;

void *send_function(void *arg)
{
    int sockfd,val,len;

    //char buffer[MAX_PKT_SIZE] = {"welcome to linux hello\0"};  //MAX_PKT_SIZE
    
    //接收线程detach自己
    pthread_detach(pthread_self());
    
    sockfd = *( (int*)arg );
    //printf("[send_function]sockfd:%d \n",sockfd);
    //fflush(stdout);
    
    val = 1;
    len = sizeof(int);
    Setsockopt(sockfd, SOL_TCP, TCP_NODELAY,(void *)&val, len);
    
   
    
    sleep(300);
    
    Close(sockfd);  
    
    return 0;
}

void *recv_function(void *arg)
{
    //接收线程detach自己
    pthread_detach(pthread_self());
    

    int sockfd;
    int num;
    char buffer[MAX_PKT_SIZE] = {0};  //MAX_PKT_SIZE
    
    
    sleep(8);
        
    sockfd = *( (int*)arg );
    //printf("[recv_function]sockfd:%d \n",sockfd);
    
    num = Read(sockfd,buffer,MAX_PKT_SIZE);
    printf("\n----------read %d bytes:%s--------------\n",num,buffer);
    
    
    return 0;
    
}

void *get_info(void *arg)
{
    int sockfd;
    int len,i=0,last_in = 0,last_out = 0;;
    struct tcp_info_user info;
    FILE *stream = fopen("rst_server","w+"); 
    
    //接收线程detach自己
    pthread_detach(pthread_self());  
      
    sockfd = *( (int*)arg );
    
    
            
    i = 0;
    while(i < 100*200000 && openflag>0)
    {
        len = sizeof(info);
        errno = 0;
        Getsockopt(sockfd, SOL_TCP, TCP_INFO,(void *)&info, (socklen_t *)&len);
        if(errno != 0)
        {
            //perror("get sock error\n");
            break;
        }
        if( (last_in != info.tcpi_segs_in) || (last_out != info.tcpi_segs_out) )
        {
            sleep_ms(1);
            len = sizeof(info);
            Getsockopt(sockfd, SOL_TCP, TCP_INFO,(void *)&info, (socklen_t *)&len);
            
            fprintftcpinfo(stream,&info);

            //printf("i=%d\n",i);
            
            
            last_in = info.tcpi_segs_in;
            last_out = info.tcpi_segs_out;
            i++;
            continue;
        }
     
        
        sleep_ms(1);
        i++;
    }  
    fclose(stream);
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
    Setsockopt(Listenfd, SOL_SOCKET, SO_REUSEPORT,(void *)&val, len);

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
        
        openflag=1;
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
        
        //printf("close\n");
        //交由系统回收关闭文件描述符
        //Close(connfd);
    }
    return 0;
}
    

