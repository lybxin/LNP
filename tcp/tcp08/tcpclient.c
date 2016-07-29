#include "../common/common.h"

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;
    struct linger so_linger;

    if (argc !=2){
        printf("argc error\n");
        exit(1);
    }

    sockfd = Socket(AF_INET,SOCK_STREAM,0);

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET,argv[1],&servaddr.sin_addr);

    Connect(sockfd,(SA*)&servaddr,sizeof(servaddr));

    so_linger.l_onoff = 1;
    so_linger.l_linger = 0;

    setsockopt(sockfd, SOL_SOCKET, SO_LINGER,
              &so_linger, sizeof so_linger);

    Close(sockfd);

    return 0;

}
    















