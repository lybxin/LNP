#include "../../common/common.h"
#include "../../common/rawops.h"
#include <pthread.h>  



void test()
{

    int sockfd, tot_len;
    unsigned char buffer[MAX_PKT_SIZE] = {"welcome to linux hello\0"};
    
    
    strncpy(srcip, "127.0.0.2", 32);
    //延迟500ms发包 模拟500ms的rtt
    senddelay = 50;
    
    //connect前设置mss为200
    mssval = (50+12);
    
    //关闭TSopt
    //tcptsopt = 0;

    sockfd = Socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
  
    initrawops(sockfd);
    //printf("---------before conn setup-------------\n");
    //rawconnect(sockfd);
    rawadvconnect(sockfd, TCP_TSOPT, TCP_TSOPT);
    //printf("---------conn setup-------------\n");
    
    
    
    //处理数据发送和业务逻辑
    tot_len = builddatapkt(buffer, recvacknumber,  strlen((const char *)buffer));
    rawsend(sockfd, buffer, tot_len);
    sleep_ms(10);
    

    
    sleep(50);
    rawconnrst(sockfd);
   
    return ;
}



int num = 10;

int main(int argc, char **argv)
{
    int i=0;
    
    if(argc==2)
    {
        //dropno = atoi(argv[1]);
    }
    
    clientport = 20001;
    for(i=0;i<num;++i,++clientport)
    {
        sleep_ms(6);
        if(fork()==0)
        {
            test();
            return 0;
        }
    
    }
    
    printf("parent ok:%u\n",clientport);

        
    sleep(3);
    


    return 0;
}


