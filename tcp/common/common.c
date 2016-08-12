#include "common.h"

int Socket(int domain, int type, int protocol)
{
    int ret;
    ret = socket(domain, type, protocol);
    if(ret < 0)
    {
        perror("socket error");
        //socket创建失败直接退出
        exit(ret);
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

int Setsockopt(int sockfd, int level, int optname,
                      const void *optval, socklen_t optlen)
{
    int ret;
    ret = setsockopt(sockfd, level, optname,optval, optlen);
    if(ret < 0)
    {
        perror("setsockopt error");
    }
    return ret;
}

int Getsockopt(int sockfd, int level, int optname,
                      void *optval, socklen_t *optlen)
{
    int ret;
    ret = getsockopt(sockfd, level, optname,optval, optlen);
    if(ret < 0)
    {
        perror("getsockopt error");
    }
    return ret;
}

ssize_t Recv(int sockfd, void *buf, size_t len, int flags)
{
    int ret;
    ret = recv(sockfd, buf, len, flags);
    if(ret < 0)
    {
        perror("recv error");
    }
    return ret;
}

ssize_t Recvfrom(int sockfd, void *buf, size_t len, int flags,
                        struct sockaddr *src_addr, socklen_t *addrlen)
{
    int ret;
    ret = recvfrom(sockfd, buf, len, flags, src_addr, addrlen);
    if(ret < 0)
    {
        perror("recvfrom error");
    }
    return ret;
}

ssize_t Send(int sockfd, const void *buf, size_t len, int flags)
{
    int ret;
    ret = send(sockfd, buf, len, flags);
    if(ret < 0)
    {
        perror("send error");
    }
    return ret;
}







