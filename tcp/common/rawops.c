#include "common.h"
#include "rawops.h"


u16 ipid = 3289;
u16 tcpseq = 8156;
u32 inittsval = 4581;
u16 mssval = 1460;
u16 tcpwz = 4096;

u32 recvtsval = 0;
u32 recvacknumber = 0;

char srcip[32] = {"127.0.0.1"};
char dstip[32] = {"127.0.0.1"};


struct pseudo_header
{
    u32 srcaddr;
    u32 dstaddr;
    u8 placeholder;
    u8 protocol;
    u16 tcplen;
};


struct thsynopts
{

    u16 msskind:8,
        msslen:8;
    u16 mssval;
    
    u16 sackpkind:8,
        sackplen:8;
        
    u16 tskind:8,
        tslen:8;
    u32 tsval;
    u32 tsecr;  
        
    u16 nop:8,
        wskind:8;
        
    u16 wslen:8,
        wsshift:8;
};

struct thnormalopts
{
        
    u16 nop1:8,
        nop2:8;
        
    u16 tskind:8,
        tslen:8;
    u32 tsval;
    u32 tsecr;  
       
};


void showseg(const unsigned char* const buffer, int buflen)
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

void initrawops()
{    
    struct timespec res;

    clock_gettime(CLOCK_REALTIME,&res);
    inittsval = (res.tv_sec&0xFFFFF) *1000 + res.tv_nsec/1000000;
    srandom(res.tv_nsec);
    //srandom(time(NULL));
    ipid = (u16)random();
    tcpseq = (u16)random();
    
    printf("init nsec:%lu,seq:%d",res.tv_nsec,tcpseq);
}

u32 gettsval()
{
    struct timespec res;
    u32 curtime;
    

    clock_gettime(CLOCK_REALTIME,&res);
    curtime = (res.tv_sec&0xFFFFF) *1000 + res.tv_nsec/1000000;
    
    //return (curtime-inittsval)>>2;
    return curtime>>2;
}

/*
    http://www.binarytides.com/raw-sockets-c-code-linux/
	Generic checksum calculation function
*/
unsigned short csum(unsigned short *ptr,int nbytes) 
{
	register long sum;
	unsigned short oddbyte;
	register short answer;

	sum=0;
	while(nbytes>1) {
		sum+=*ptr++;
		nbytes-=2;
	}
	if(nbytes==1) {
		oddbyte=0;
		*((u_char*)&oddbyte)=*(u_char*)ptr;
		sum+=oddbyte;
	}

	sum = (sum>>16)+(sum & 0xffff);
	sum = sum + (sum>>16);
	answer=(short)~sum;
	
	return(answer);
}


//https://bytes.com/topic/c/answers/218941-help-raw-socket-checksum
 /* this function generates header checksums */
unsigned short csum01(unsigned short *buf, int nwords)
{
    unsigned long sum;
    
    for (sum = 0; nwords > 0; nwords--)
        sum += *buf++;
        
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    
    return ~sum;
}

//注意checksum和tot_len
void filliphdr(struct iphdr *iph, u16 tot_len)
{
    //Fill in the IP Header
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = htons(tot_len);
    //iph->tot_len = sizeof (struct iphdr) + sizeof (struct tcphdr) + strlen(data);
    iph->id = htons(ipid); //Id of this packet
    iph->frag_off = htons(0);
    iph->ttl = 255;
    iph->protocol = IPPROTO_TCP;
    iph->check = htons(0);      //Set to 0 before calculating checksum
    iph->saddr = inet_addr(srcip);    //Spoof the source ip address
    iph->daddr = inet_addr(dstip);;
    
    iph->check = htons(csum ((unsigned short *) iph, iph->tot_len*4));
}

//头长doff check  确保optlen四字节对齐
void filltcphdr(struct tcphdr *th, int syn, int ack, u32 seq, u32 acknumber, u16 optlen)
{
	
    //TCP Header
    th->source = htons (SERV_PORT02);
    th->dest = htons (SERV_PORT01);
    th->seq = htonl(seq);
    th->ack_seq = htonl(acknumber);
    th->doff = optlen/4+5;  //tcp header size
    th->fin=0;
    th->syn=syn;
    th->rst=0;
    th->psh=0;
    th->ack=ack;
    th->urg=0;
    //th->ece=0;
    //th->cwr=0;
    th->window = htons(tcpwz); /* maximum allowed window size */
    th->check = 0; //leave checksum 0 now, filled later by pseudo header
    th->urg_ptr = 0;
}


void updatetcphdr(struct tcphdr *th, u16 optdatalen)
{
    struct pseudo_header psh;
    char *psddata;
    
    //Now the TCP checksum
    psh.srcaddr = inet_addr(srcip);
    psh.dstaddr = inet_addr(dstip);
    psh.placeholder = 0;
    psh.protocol = IPPROTO_TCP;
    psh.tcplen = htons(sizeof(struct tcphdr) + optdatalen );
     
    int psize = sizeof(struct pseudo_header) + sizeof(struct tcphdr) + optdatalen;
    psddata = malloc(psize);
    
    memcpy(psddata , (char*) &psh , sizeof (struct pseudo_header));
    memcpy(psddata + sizeof(struct pseudo_header) , th , sizeof(struct tcphdr) + optdatalen);
    
    th->check = csum( (unsigned short*) psddata , psize);
    
}

void fillsynopts(struct tcphdr* th)
{
    struct thsynopts *options;
    
    options = (struct thsynopts*)((u8*)th + 20);
    
    options->msskind = 2;
    options->msslen = 4;
    options->mssval = htons(mssval);
    
    options->sackpkind = 4;
    options->sackplen = 2;
    
    options->tskind = 8;
    options->tslen = 10;
    options->tsval = htonl(gettsval());
    options->tsecr = 0;
    
    options->nop = 1;
    
    options->wskind = 3;
    options->wslen = 3;
    options->wsshift = 0;

}

//确保buffer len足够长
u16 buildsynseg(u8 *buffer)
{
    struct iphdr *iph;
    struct tcphdr *th;
    u16 tot_len;
    
    iph = (struct iphdr*)buffer;
    
    tot_len = sizeof(struct iphdr)+sizeof(struct tcphdr)+sizeof(struct thsynopts);
    
    filliphdr(iph, tot_len);
    
    th = (struct tcphdr*)(buffer + iph->ihl * 4);
    
    filltcphdr(th, 1, 0, tcpseq, 0, sizeof(struct thsynopts));
    
    fillsynopts(th);
    
    updatetcphdr(th, sizeof(struct thsynopts) );
    
    return tot_len;
}


//tsecr 需要recv更新
void fillnormalopts(struct tcphdr* th)
{
    struct thnormalopts *options;
    
    options = (struct thnormalopts*)((u8*)th + 20);
    
    options->nop1 = 1;
    options->nop2 = 1;
    
    options->tskind = 8;
    options->tslen = 10;
    options->tsval = htonl(gettsval());
    options->tsecr = htonl(recvtsval);

}


//确保buffer len足够长
u16 buildackseg(u8 *buffer)
{
    struct iphdr *iph;
    struct tcphdr *th;
    u16 tot_len;
    
    iph = (struct iphdr*)buffer;
    
    tot_len = sizeof(struct iphdr)+sizeof(struct tcphdr)+sizeof(struct thnormalopts);
    
    filliphdr(iph, tot_len);
    
    th = (struct tcphdr*)(buffer + iph->ihl * 4);
    
    filltcphdr(th, 0, 1, tcpseq, recvacknumber, sizeof(struct thnormalopts));
    
    fillnormalopts(th);
    
    updatetcphdr(th, sizeof(struct thnormalopts) );
    
    return tot_len;
}

//确保buffer len足够长
u16 buildrstseg(u8 *buffer)
{
    struct iphdr *iph;
    struct tcphdr *th;
    u16 tot_len;
    
    iph = (struct iphdr*)buffer;
    
    tot_len = sizeof(struct iphdr)+sizeof(struct tcphdr)+sizeof(struct thnormalopts);
    
    filliphdr(iph, tot_len);
    
    th = (struct tcphdr*)(buffer + iph->ihl * 4);
    
    filltcphdr(th, 0, 1, tcpseq, recvacknumber, sizeof(struct thnormalopts));
    
    th->rst = 1;
    
    fillnormalopts(th);
    
    updatetcphdr(th, sizeof(struct thnormalopts) );
    
    return tot_len;
}


void updaterecvstate(u8 *buffer, u16 recvlen)
{
    struct iphdr *iph;
    struct tcphdr *th;
    u16 datalen;
    u8 *options, *optsend;
    
    iph = (struct iphdr*)buffer;
    th = (struct tcphdr*)(buffer + iph->ihl * 4);
    
    showseg((u8*)th,48);
    
    datalen = recvlen - iph->ihl*4 - th->doff*4;
    
    recvacknumber = ntohl(th->seq) + th->syn + th->fin + datalen;
    
    printf("recv seq:%u,syn:%u,fin:%u,datalen:%u\n",ntohl(th->seq),th->syn,th->fin,datalen);
    
    
    options = buffer + iph->ihl * 4 + 20;
    optsend = buffer + iph->ihl * 4 + (th->doff * 4);
    
    while (options < optsend)
    {
        //EOL
        if(*options == 0)
           break;
           
       //NOP
       if(*options == 1)
       {
           options++;
           continue;   
       }
       //TSopt
       if(*options == 8)
       {           
           recvtsval = ntohl(*((u32*)(options + 2)));
           printf("parse tsval:%u loc:%lu, th:%lu\n",recvtsval,(long)options,(long)th);
           showseg(options,16);
           break;
       }
       
       //other options
       options++;
       options = options + (*options) - 1;
    }
    
}

u32 segforme(u8 *buffer)
{    
    struct iphdr *iph;
    struct tcphdr *th;
    
    iph = (struct iphdr*)buffer;
    th = (struct tcphdr*)(buffer + iph->ihl * 4);
    printf("segforme dest:%d\n",ntohs(th->dest));
    return ( th->dest == htons (SERV_PORT02) );

}


u16 rawrecv(int sockfd, u8 *buffer, u16 buflen)
{
    u16 recvlen;
    u16 len;
    //socklen_t socklen;
    //struct sockaddr_in in_addr;
    
    //socklen=sizeof(struct sockaddr_in);
    while(1)
    {
        len = buflen;
        recvlen = Recv(sockfd, buffer, len, 0);
                   // (struct sockaddr *)&in_addr, &socklen);
        if(segforme(buffer))
            break;
    }

                       
                       
    showseg((u8*)buffer,60);
    printf("before update rcv state\n");
                        
    updaterecvstate(buffer, recvlen);
                        
    return recvlen;
}

u32 issynseg(u8 *buffer, u16 buflen)
{
    struct iphdr *iph;
    struct tcphdr *th;
    
    iph = (struct iphdr*)buffer;
    th = (struct tcphdr*)(buffer + iph->ihl * 4);
    
    return th->syn;
}

void updatesendstate(u8 *buffer, u16 sendlen)
{
    struct iphdr *iph;
    struct tcphdr *th;
    u16 datalen;
    
    iph = (struct iphdr*)buffer;
    th = (struct tcphdr*)(buffer + iph->ihl * 4);
         
    datalen = sendlen - iph->ihl*4 - th->doff*4;
    
    tcpseq = ntohl(th->seq) + th->syn + th->fin + datalen;
    
    ipid++;
}

u16 rawsend(int sockfd, u8 *buffer, u16 buflen)
{
    u16 sendlen;
    //socklen_t socklen;
    //struct sockaddr_in in_addr;
    
    //socklen=sizeof(struct sockaddr_in);
    
    sendlen = Send(sockfd, buffer, buflen, 0);
                       // (struct sockaddr *)&in_addr, &socklen);
                        
    updatesendstate(buffer, sendlen);
                        
    return sendlen;
}


//返回错误需要处理
int rawconnect(int sockfd)
{
    u8 buffer[MAX_SEG_SIZE];
    u16 tot_len;
    
    tot_len = buildsynseg(buffer);
    
    showseg(buffer,tot_len);
    
    rawsend(sockfd,buffer,tot_len);
    
    rawrecv(sockfd, buffer, MAX_SEG_SIZE);
    
    tot_len = buildackseg(buffer);
    
    rawsend(sockfd,buffer,tot_len);
    
    sleep(30);
    
    tot_len = buildrstseg(buffer);
    
    rawsend(sockfd,buffer,tot_len);
    
    return 0;

}










