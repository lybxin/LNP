#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<time.h>
#include "common.h"

//void str_cli(FILE *fp,int sockfd);

int main(int argc, char **argv)
{
    int sockfd, ret,count=0;
    struct sockaddr_in servaddr;
    struct timespec now,res;
    char writebuf[TRANSSIZE];
    int i;


    if (argc !=2){
        printf("argc error\n");
        exit(1);
    }
    
    for(i = 0; ; i++)
    {
        sockfd = socket(AF_INET,SOCK_STREAM,0);

        memset(&servaddr,0,sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(SERV_PORT);
        inet_pton(AF_INET,argv[1],&servaddr.sin_addr);

        ret = connect(sockfd,(SA*)&servaddr,sizeof(servaddr));
        if(ret == 0){
            count++;
            printf("connect success:%d count:%d\n",i,count);
        }
        else{
            printf("connect error:%d count:%d fd:%d   ",i,count,sockfd);

            perror("error");
            break;
        }

    }

    //进程退出时候  操作系统自动释放文件描述符号  此处不在记录并释放
    //close(sockfd);

    exit(0);

}
    















