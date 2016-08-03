#include "../common/common.h"

void showpacket(const unsigned char* const buffer);


int main(int argc, char **argv)
{
    int sockfd,socklen,recvlen,pktnum;
    unsigned char buffer[65536];
    
    struct sockaddr_in in_addr;
    
    if(argc == 2)
    {
        pktnum = atoi(argv[1]);
    }else
    {
        pktnum = 3;
    }

    
    socklen=sizeof(struct sockaddr_in);
    
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    //sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP|IPPROTO_UDP|IPPROTO_ICMP);
    perror("socket");
    while(pktnum > 0)
    {
        printf("recv start\n");
        recvlen = recv(sockfd, buffer, sizeof(buffer), 0);
                        //(struct sockaddr *)&in_addr, &socklen);
        perror("recvfrom");
        pktnum--;
    
    }

    showpacket(buffer);

    return 0;
}


void showpacket(const unsigned char* const buffer, int buflen)
{
   int i=0;

   for(i=0;i<buflen;i++)
   {
       if(i==25)printf("\n");
           printf("%2x ",buffer[i]);
   }
   printf("\n-------------------------\n");
}
