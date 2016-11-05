#ifndef	_COMMON_H
#define	_COMMON_H	

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
#include<netinet/tcp.h>
#include<netinet/ip.h>
//#include<netinet/in.h>


#define SERV_PORT 9877
#define LISTENQ 1024
#define SA struct sockaddr
#define TRANSSIZE 10


int Socket(int domain, int type, int protocol);

int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

int Listen(int sockfd, int backlog);

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

int Shutdown(int sockfd, int how);

int Close(int fd);

ssize_t Write(int fd, const void *buf, size_t count);

ssize_t Read(int fd, void *buf, size_t count);

int Setsockopt(int sockfd, int level, int optname,
                      const void *optval, socklen_t optlen);
                      
int Getsockopt(int sockfd, int level, int optname,
                      void *optval, socklen_t *optlen);
            
ssize_t Recv(int sockfd, void *buf, size_t len, int flags);

ssize_t Recvfrom(int sockfd, void *buf, size_t len, int flags,
                        struct sockaddr *src_addr, socklen_t *addrlen);
                                             
ssize_t Send(int sockfd, const void *buf, size_t len, int flags);

void initskaddr(struct sockaddr_in *skaddr, char * addr, unsigned short port);


                        
#endif



