#include<stdio.h>
#include<time.h>
#include<string.h>  //memset
#include<stdlib.h>  //exit
#include<unistd.h>  //fork close
//#include <netinet/in.h>   //addr_in?  yes
#include<arpa/inet.h>  //hton* ntoh*
#include<time.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<errno.h>

#define SERV_PORT 9877
#define LISTENQ 1024
#define SA struct sockaddr
#define TRANSSIZE 10


int Socket(int domain, int type, int protocol)
{
    int ret;
    ret = socket(domain, type, protocol);
    if(ret < 0)
    {
        perror("socket error");
    }
}



int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    int ret;
    ret = bind(sockfd, addr, addrlen);
    if(ret < 0)
    {
        perror("bind error");
    }
}

int Listen(int sockfd, int backlog)
{
    int ret;
    ret = listen(sockfd, backlog);
    if(ret < 0)
    {
        perror("listen error");
    }
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    int ret;
    ret = accept(sockfd, addr, addrlen);
    if(ret < 0)
    {
        perror("accept error");
    }
}


int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    int ret;
    ret = connect(sockfd, addr, addrlen);
    if(ret < 0)
    {
        perror("connect error");
    }
}

int Shutdown(int sockfd, int how)
{
    int ret;
    ret = shutdown(sockfd, how);
    if(ret < 0)
    {
        perror("shutdown error");
    }
}

int Close(int fd)
{
    int ret;
    ret = close(fd);
    if(ret < 0)
    {
        perror("close error");
    }
}

ssize_t Write(int fd, const void *buf, size_t count)
{
    int ret;
    ret = write(fd, buf, count);
    if(ret < 0)
    {
        perror("write error");
    }
}

ssize_t Read(int fd, void *buf, size_t count)
{
    int ret;
    ret = read(fd, buf, count);
    if(ret < 0)
    {
        perror("read error");
    }
}






