#include "../../common/common.h"
#include "../../common/rawops.h"


void showpacket(const unsigned char* const buffer, int buflen);


int main(int argc, char **argv)
{
    int sockfd,recvlen,pktnum;
    socklen_t socklen;
    struct sockaddr_in bindaddr, servaddr;
    unsigned char buffer[65536];
    struct iphdr *iph;
    struct tcphdr *th;
    
    int optval = 1;
    
    struct sockaddr_in in_addr;
    
    if(argc == 2)
    {
        pktnum = atoi(argv[1]);
    }else
    {
        pktnum = 3;
    }

    
    socklen=sizeof(struct sockaddr_in);
    
    sockfd = Socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    //sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP|IPPROTO_UDP|IPPROTO_ICMP);
    //perror("socket");
    
    Setsockopt(sockfd,IPPROTO_IP, IP_HDRINCL, &optval, sizeof(optval));
    
    
    memset(&bindaddr,0,sizeof(bindaddr));
    bindaddr.sin_family = AF_INET;
    bindaddr.sin_addr.s_addr = inet_addr(srcip);
    bindaddr.sin_port = htons(SERV_PORT01);
    Bind(sockfd,(SA*)&bindaddr,sizeof(bindaddr));
    
    
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    servaddr.sin_addr.s_addr = inet_addr(dstip);
    //inet_pton(AF_INET,argv[1],&servaddr.sin_addr);

    Connect(sockfd,(SA*)&servaddr,sizeof(servaddr));
    
    
    initrawops();
    rawconnect(sockfd);
    
    /*   
    while(pktnum > 0)
    {
        printf("--------recvfrom start:%d-----------\n",pktnum);
        recvlen = Recvfrom(sockfd, buffer, sizeof(buffer), 0,
                        (struct sockaddr *)&in_addr, &socklen);
        iph = (struct iphdr*)buffer;
        th = (struct tcphdr*)(buffer + iph->ihl*4);
        printf("ip ver:%d,ihl:%d tcp srcport:%d,dstport:%d\n",iph->version,iph->ihl,ntohs(th->source),ntohs(th->dest));
        printf("tcp syn:%d,ack:%d,acknumber:%d  recv:%d\n",th->syn,th->ack,ntohl(th->ack_seq),recvlen);
        //perror("recvfrom");
        
        showpacket(buffer,60);
        
        pktnum--;
    
    }
    */

    
    return 0;
}


void showpacket(const unsigned char* const buffer, int buflen)
{
   int i=0;

   for(i=0;i<buflen;i++)
   {
       if(i%8 ==0)
       {
           if(i%16 == 0)
               printf("\n");
           else
               printf("\t");  
       }        
               
       printf("%02x ",buffer[i]);
   }
   printf("\n-------------------------\n");
}
