#include<stdio.h>
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
