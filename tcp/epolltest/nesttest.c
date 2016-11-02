#include "../common/common.h"
#include "../common/rawops.h"
#include "../common/tcp_private.h"

#include <sys/epoll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>


#define MAX_EVENTS 10
#define STR(R)  #R 

#define addfd(epfd,fd)  \
do{\
    printf("add " STR(fd) " to " STR(epfd) ":" );\
    addfdproc(epfd,fd);\
}while(0)



void addfdproc(int epfd, int t_fd)
{
    struct epoll_event ev;
    static int num;
    
    ev.events = EPOLLIN;
    ev.data.fd = t_fd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, t_fd, &ev) == -1) {
       printf("epoll_ctl error:%s(errno:%d)\n",strerror(errno),errno);
       return;
       //exit(EXIT_FAILURE);
    }
    num++;
    printf("add %d success \n",num);
    
}

void test1()
{
    int epfd1,epfd2,epfd3,epfd4,epfd5,epfd6,epfd7,epfd8,epfd9;
    
    
    epfd1 = epoll_create1(0);
    epfd2 = epoll_create1(0);
    epfd3 = epoll_create1(0);
    epfd4 = epoll_create1(0);
    epfd5 = epoll_create1(0);
    epfd6 = epoll_create1(0);
    epfd7 = epoll_create1(0);
    epfd8 = epoll_create1(0);
    epfd9 = epoll_create1(0);
    
    printf("---------------test1 start--------------- \n");
    addfd(epfd1,epfd2);
    addfd(epfd2,epfd3);
    addfd(epfd3,epfd4);
    addfd(epfd4,epfd5);
    addfd(epfd5,epfd6);
    addfd(epfd6,epfd7);
    addfd(epfd7,epfd8);
    addfd(epfd8,epfd9);
    printf("---------------test1 end--------------- \n\n");
}

void test2()
{
    int epfd1,epfd2,epfd3,epfd4,epfd5,epfd6,epfd7,epfd8,epfd9;
    
    
    epfd1 = epoll_create1(0);
    epfd2 = epoll_create1(0);
    epfd3 = epoll_create1(0);
    epfd4 = epoll_create1(0);
    epfd5 = epoll_create1(0);
    epfd6 = epoll_create1(0);
    epfd7 = epoll_create1(0);
    epfd8 = epoll_create1(0);
    epfd9 = epoll_create1(0);
    
    printf("---------------test2 start--------------- \n");
    addfd(epfd2,epfd1);
    addfd(epfd3,epfd2);
    addfd(epfd4,epfd3);
    addfd(epfd5,epfd4);
    addfd(epfd6,epfd5);
    addfd(epfd7,epfd6);
    addfd(epfd8,epfd7);
    addfd(epfd9,epfd8);
    printf("---------------test2 end--------------- \n\n");
}

void test3()
{
    int epfd1,epfd2,epfd3,epfd4,epfd5,epfd6,epfd7,epfd8,epfd9;
    
    
    epfd1 = epoll_create1(0);
    epfd2 = epoll_create1(0);
    epfd3 = epoll_create1(0);
    epfd4 = epoll_create1(0);
    epfd5 = epoll_create1(0);
    epfd6 = epoll_create1(0);
    epfd7 = epoll_create1(0);
    epfd8 = epoll_create1(0);
    epfd9 = epoll_create1(0);
    
    printf("---------------test3 start--------------- \n");
    addfd(epfd1,epfd2);
    addfd(epfd2,epfd3);
    addfd(epfd3,epfd1);
    printf("---------------test3 end--------------- \n\n");
}



void test4()
{
    int epfd1,epfd2,epfd3,epfd4,epfd5,epfd6,epfd7,epfd8,epfd9,listen_fd;
    struct sockaddr_in servaddr;
    
    listen_fd = socket(AF_INET,SOCK_STREAM,0);

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port =  htons(9877);
    
    bind(listen_fd,(SA*)&servaddr,sizeof(servaddr));
    listen(listen_fd,LISTENQ);
    
    epfd1 = epoll_create1(0);
    epfd2 = epoll_create1(0);
    epfd3 = epoll_create1(0);
    epfd4 = epoll_create1(0);
    epfd5 = epoll_create1(0);
    epfd6 = epoll_create1(0);
    epfd7 = epoll_create1(0);
    epfd8 = epoll_create1(0);
    epfd9 = epoll_create1(0);
    
    printf("---------------test4 反向查找--------------- \n");
    addfd(epfd1,epfd2);
    addfd(epfd2,epfd3);
    addfd(epfd3,epfd4);
    addfd(epfd4,listen_fd);
    addfd(epfd4,epfd5);
    addfd(epfd5,listen_fd);
    addfd(epfd5,epfd6);
    addfd(epfd6,listen_fd);
    addfd(epfd6,epfd7);
    addfd(epfd7,listen_fd);
    addfd(epfd7,epfd8);
    addfd(epfd8,epfd9);
    addfd(epfd9,listen_fd);
    printf("---------------test4 end--------------- \n\n");
}

void test5()
{
    int  i=0,sum=1030,listen_fd;
    struct sockaddr_in servaddr;
    struct epoll_event ev;
    int epfd[1030]={-1};


    

    listen_fd = socket(AF_INET,SOCK_STREAM,0);

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port =  htons(9877);
    
    bind(listen_fd,(SA*)&servaddr,sizeof(servaddr));
    listen(listen_fd,LISTENQ);
    
    
    printf("---------------test5 反向查找 i:%d --------------- \n",i);

    while(i<sum)
    {
        
        epfd[i]=epoll_create1(0);
        ev.events = EPOLLIN;
        ev.data.fd = listen_fd;
        if (epoll_ctl(epfd[i], EPOLL_CTL_ADD, listen_fd, &ev) == -1) {
            printf("添加第一层 add error num:%d  error:%s(errno:%d)\n",i+1,strerror(errno),errno);
            break;
        }
        ++i;
        
    }

    
    printf("---------------test5 end i:%d --------------- \n\n",i);
}


void test6()
{
    int  i=0,sum=510,listen_fd;
    struct sockaddr_in servaddr;
    struct epoll_event ev;
    int epfd[1024]={-1};
    int epfd2[1024]={-1};

    

    listen_fd = socket(AF_INET,SOCK_STREAM,0);

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port =  htons(9877);
    
    bind(listen_fd,(SA*)&servaddr,sizeof(servaddr));
    listen(listen_fd,LISTENQ);
    
    
    printf("---------------test6 反向查找 i:%d --------------- \n",i);

    while(i<sum)
    {
        
        epfd[i]=epoll_create1(0);
        ev.events = EPOLLIN;
        ev.data.fd = listen_fd;
        if (epoll_ctl(epfd[i], EPOLL_CTL_ADD, listen_fd, &ev) == -1) {
            printf("add error num:%d  error:%s(errno:%d)\n",i+1,strerror(errno),errno);
            break;
        }
        ++i;
        
    }
    
    i=0;
    while(i<sum)
    {
        
        epfd2[i]=epoll_create1(0);
        ev.events = EPOLLIN;
        ev.data.fd = epfd[i];
        if (epoll_ctl(epfd2[i], EPOLL_CTL_ADD, epfd[i], &ev) == -1) {
            printf("第二层 add error i:%d  error:%s(errno:%d)\n",i+1,strerror(errno),errno);
            break;
        }
        ++i;
        
    }
    
    
    
    
    printf("---------------test6 end i:%d --------------- \n\n",i);
}


void main()
{
/*
    if(fork()==0)
    {
        
        test1();
        return;
    }
    
    sleep(1);
    
    if(fork()==0)
    {
        test2();
        return;
    }
    
    sleep(1);
    
    if(fork()==0)
    {
        test3();
        return;
    }
    
    sleep(1);
 */    
    if(fork()==0)
    {
        test4();
        return;
    }
    
    sleep(1);
   
    if(fork()==0)
    {
        test5();
        return;
    }
    sleep(1);
    
    if(fork()==0)
    {
        test6();
        return;
    }
    
    sleep(1);

}




