#include "common.h"

int Socket(int domain, int type, int protocol)
{
    int ret;
    ret = socket(domain, type, protocol);
    if(ret < 0)
    {
        perror("socket error");
    }
    return ret;
}



int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    int ret;
    ret = bind(sockfd, addr, addrlen);
    if(ret < 0)
    {
        perror("bind error");
    }
    return ret;
}

int Listen(int sockfd, int backlog)
{
    int ret;
    ret = listen(sockfd, backlog);
    if(ret < 0)
    {
        perror("listen error");
    }
    return ret;
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    int ret;
    ret = accept(sockfd, addr, addrlen);
    if(ret < 0)
    {
        perror("accept error");
    }
    return ret;
}


int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    int ret;
    ret = connect(sockfd, addr, addrlen);
    if(ret < 0)
    {
        perror("connect error");
    }
    return ret;
}

int Shutdown(int sockfd, int how)
{
    int ret;
    ret = shutdown(sockfd, how);
    if(ret < 0)
    {
        perror("shutdown error");
    }
    return ret;
}

int Close(int fd)
{
    int ret;
    ret = close(fd);
    if(ret < 0)
    {
        perror("close error");
    }
    return ret;
}

ssize_t Write(int fd, const void *buf, size_t count)
{
    int ret;
    
    ret = write(fd, buf, count);
    if(ret < 0)
    {
        perror("write error");
    }
    
    return ret;
}

ssize_t Read(int fd, void *buf, size_t count)
{
    int ret;
    ret = read(fd, buf, count);
    if(ret < 0)
    {
        perror("read error");
    }
    return ret;
}

