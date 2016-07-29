#include "../common/common.h"

int main(int argc, char **argv)
{
    int sockfd,ret;
    struct sockaddr_in servaddr;
    char writebuf[TRANSSIZE];

    if (argc !=2){
        printf("argc error\n");
        exit(1);
    }


    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET,argv[1],&servaddr.sin_addr);

    //第一次触发正常连接 获取FOC
    printf("first Connect");
    sockfd = Socket(AF_INET,SOCK_STREAM,0);
    snprintf(writebuf,TRANSSIZE,"hello");
    ret = sendto(sockfd, writebuf, strlen(writebuf), 
		MSG_FASTOPEN, (SA*)&servaddr, sizeof(servaddr)); 
    printf("client ret %d\n",ret);
    Close(sockfd);

    //第二次触发TFO
    sockfd = Socket(AF_INET,SOCK_STREAM,0);
    snprintf(writebuf,TRANSSIZE,"world");
    ret = sendto(sockfd,writebuf,strlen(writebuf),
            MSG_FASTOPEN,(SA*)&servaddr,sizeof(servaddr));
    Close(sockfd);

    return 0;

}
    















