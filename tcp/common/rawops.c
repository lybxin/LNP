#include "common.h"
#include "rawops.h"


u16 ipid = 3289;
u16 tcpseq = 8156;
//u32 inittsval = 4581;
u16 mssval = 1460;
u16 tcpwz = 4096;


u32 recvtsval = 0;
u32 recvacknumber = 0;

u32 senddelay = 0;   //�ӳٷ��ͱ��� ��λms

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

//����ʹ��  ��wireshark�����ʽ��ͬ ����Ա�
void showpkt(const unsigned char* const buffer, int buflen)
{
   int i=0;
   printf("\n----------------------showpkt start----------------------\n");
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
   printf("\n-----------------------showpkt end-----------------------\n");
}

void initrawops(int sockfd)
{    
    int optval = 1;
    struct sockaddr_in bindaddr, servaddr;
    
    //��ʼ��ipid��tcpseq
    srandom(time(NULL));
    ipid = (u16)random();
    tcpseq = (u16)random();
    
    printf("[initrawops] ipid:%u,tcpseq:%u\n",ipid,tcpseq);
    
    Setsockopt(sockfd,IPPROTO_IP, IP_HDRINCL, &optval, sizeof(optval));
    
    //�󶨱��ؽ��յ�ַ  ֻ���շ���srcip�ı���
    memset(&bindaddr,0,sizeof(bindaddr));
    bindaddr.sin_family = AF_INET;
    bindaddr.sin_addr.s_addr = inet_addr(srcip);
    //bindaddr.sin_port = htons(SERV_PORT01);
    Bind(sockfd,(SA*)&bindaddr,sizeof(bindaddr));
    
    //�󶨱��ط��͵�ַ  ʹ��sendֱ�ӷ��ͱ���
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    servaddr.sin_addr.s_addr = inet_addr(dstip);
    //inet_pton(AF_INET,argv[1],&servaddr.sin_addr);
    Connect(sockfd,(SA*)&servaddr,sizeof(servaddr));
}

//��ȡTSopt�е�tsval  ÿ1ms����1
u32 gettsval()
{
    struct timespec res;
    u32 tsval;
    
    clock_gettime(CLOCK_REALTIME,&res);
    tsval = (res.tv_sec&0xFFFFF) *1000 + res.tv_nsec/1000000;

    return tsval;
}

/*
    http://www.binarytides.com/raw-sockets-c-code-linux/
	Generic checksum calculation function
	<TCP/IP Illustrated> P186
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

//ע��checksum��tot_len
void filliphdr(struct iphdr *iph, u16 tot_len)
{
    //Fill in the IP Header
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = htons(tot_len);
    iph->id = htons(ipid);      //Id of this packet
    iph->frag_off = htons(0);
    iph->ttl = 255;
    iph->protocol = IPPROTO_TCP;
    iph->check = htons(0);      //Set to 0 before calculating checksum
    iph->saddr = inet_addr(srcip); 
    iph->daddr = inet_addr(dstip);;
    
    iph->check = htons(csum ((unsigned short *) iph, iph->tot_len*4));
}

//ͷ��doff check  ȷ��optlen���ֽڶ���
void filltcphdr(struct tcphdr *th, int syn, int ack, u32 seq, u32 acknumber, u16 optlen)
{
	
    //TCP Header
    th->source = htons (SERV_PORT02);
    th->dest = htons (SERV_PORT01);
    th->seq = htonl(seq);
    th->ack_seq = htonl(acknumber);
    th->doff = optlen/4+5;        //tcp header size
    th->fin=0;
    th->syn=syn;
    th->rst=0;
    th->psh=0;
    th->ack=ack;
    th->urg=0;
    //th->ece=0;
    //th->cwr=0;
    th->window = htons(tcpwz);    
    th->check = 0;                 //����checksumǰ ��Ҫ�Ƚ�check����Ϊ0
    th->urg_ptr = 0;
}

//����tcp checksum
void updatetcphdr(struct tcphdr *th, u16 optdatalen)
{
    struct pseudo_header psh;
    char *psddata;
    int psize;
    
    //Now the TCP checksum
    psh.srcaddr = inet_addr(srcip);
    psh.dstaddr = inet_addr(dstip);
    psh.placeholder = 0;
    psh.protocol = IPPROTO_TCP;
    psh.tcplen = htons(sizeof(struct tcphdr) + optdatalen );
     
    psize = sizeof(struct pseudo_header) + sizeof(struct tcphdr) + optdatalen;
    psddata = malloc(psize);
    
    memcpy(psddata , (char*) &psh , sizeof (struct pseudo_header));
    memcpy(psddata + sizeof(struct pseudo_header) , th , sizeof(struct tcphdr) + optdatalen);
    
    th->check = csum( (unsigned short*) psddata , psize);
    
    free(psddata);
}

//���SYN����TCPѡ��
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

//����SYN���� ���ȷ��buffer len�㹻��
u16 buildsynpkt(u8 *buffer)
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


//���ACK���ĺ���ͨ���ݱ��ĵ�TCPѡ�� tsecr ��Ҫrecv����
//���SACK֧�ֵ�ʱ�� ��Ҫע��sizeof(struct thnormalopts)
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


//���ȷ��buffer len�㹻��
u16 buildackpkt(u8 *buffer, u32 acknumber, u32 flag)
{
    struct iphdr *iph;
    struct tcphdr *th;
    u16 tot_len, optlen = 0;
    
    if(flag&TCP_TSOPT)
    {
        optlen = sizeof(struct thnormalopts);
    }
    
    iph = (struct iphdr*)buffer;
    
    tot_len = sizeof(struct iphdr)+sizeof(struct tcphdr)+ optlen;
    
    filliphdr(iph, tot_len);
    
    th = (struct tcphdr*)(buffer + iph->ihl * 4);
    
    filltcphdr(th, 0, 1, tcpseq, acknumber, optlen);
    
    if(flag&TCP_TSOPT)
    {
        fillnormalopts(th);
    }
    
    //printf("[buildackpkt] optlen:%u,tot_len:%u\n",optlen,tot_len);
    
    updatetcphdr(th, optlen);
        
    return tot_len;
}

//���ȷ��buffer len�㹻��  �������hdr���tot_len
u16 builddatapkt(u8 *buffer, u32 acknumber,u16 buflen)
{
    struct iphdr *iph;
    struct tcphdr *th;
    u16 tot_len,hdrlen;
    
    hdrlen = sizeof(struct iphdr)+sizeof(struct tcphdr)+sizeof(struct thnormalopts);
    
    memmove((buffer + hdrlen), buffer, buflen);
    
    iph = (struct iphdr*)buffer;
    
    tot_len = hdrlen + buflen;
    
    filliphdr(iph, tot_len);
    
    th = (struct tcphdr*)(buffer + iph->ihl * 4);
    
    filltcphdr(th, 0, 1, tcpseq, acknumber, sizeof(struct thnormalopts));
    
    fillnormalopts(th);
    
    updatetcphdr(th, ( sizeof(struct thnormalopts) + buflen ) );
    
    return tot_len;
}

//���ȷ��buffer len�㹻��
u16 buildrstpkt(u8 *buffer)
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
    
    datalen = recvlen - iph->ihl*4 - th->doff*4;
    recvacknumber = ntohl(th->seq) + th->syn + th->fin + datalen;
        
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
           break;
       }
       
       //other options
       options++;
       options = options + (*options) - 1;
    }
    
    printf("[updaterecvstate]seq:%u,syn:%u,fin:%u,datalen:%u,acknumber:%u,recvtsval:%u\n",
             ntohl(th->seq),th->syn,th->fin,datalen,recvacknumber,recvtsval);
    
}

//�ж�Ŀ�Ķ˿��Ƿ�ΪҪ����Ķ˿�
u32 pktforme(u8 *buffer)
{    
    struct iphdr *iph;
    struct tcphdr *th;
    
    iph = (struct iphdr*)buffer;
    th = (struct tcphdr*)(buffer + iph->ihl * 4);
    
    return ( th->dest == htons (SERV_PORT02) );
}


u16 rawrecv(int sockfd, u8 *buffer, u16 buflen)
{
    u16 recvlen;
    u16 len;

    while(1)
    {
        len = buflen;
        recvlen = Recv(sockfd, buffer, len, 0);
        
        if(recvlen < 0)
        {
            return recvlen;
        }
        
        //bind����ֻ�ܹ���ip��ַ  �˴���һ�����˶˿�
        if(pktforme(buffer))
            break;
    }
               
    printf("\n[rawrecv]before update rcv state\n");                  
    showpkt((u8*)buffer,60);
                        
    updaterecvstate(buffer, recvlen);
                        
    return recvlen;
}

/*
u32 issynpkt(u8 *buffer, u16 buflen)
{
    struct iphdr *iph;
    struct tcphdr *th;
    
    iph = (struct iphdr*)buffer;
    th = (struct tcphdr*)(buffer + iph->ihl * 4);
    
    return th->syn;
}
*/

//���ȷ��sendlen��Ч
void updatesendstate(u8 *buffer, u16 sendlen)
{
    struct iphdr *iph;
    struct tcphdr *th;
    u16 datalen;
    
    iph = (struct iphdr*)buffer;
    th = (struct tcphdr*)(buffer + iph->ihl * 4);
         
    datalen = sendlen - iph->ihl*4 - th->doff*4;
    
    //������һ��pkt��seq
    tcpseq = ntohl(th->seq) + th->syn + th->fin + datalen;
    //����ipid
    ipid++;
}

void sleep_ms(int timeval)
{
    struct timespec req;
    req.tv_sec = senddelay/1000;
    req.tv_nsec = (senddelay%1000) * 1000000;
    clock_nanosleep(CLOCK_MONOTONIC,0,&req,NULL);

}

u16 rawsend(int sockfd, u8 *buffer, u16 buflen)
{
    u16 sendlen;
    
    
    if(senddelay > 0)
    {
        sleep_ms(senddelay);
    }
    
    sendlen = Send(sockfd, buffer, buflen, 0);
    
    printf("\n[rawsend]before updatesendstate\n");
    showpkt((u8*)buffer,60);
                        
    updatesendstate(buffer, sendlen);
                        
    return sendlen;
}


//���ش�����Ҫ����
int rawconnect(int sockfd)
{
    u8 buffer[MAX_PKT_SIZE];
    u16 tot_len;
    
    //����SYN
    tot_len = buildsynpkt(buffer);
    rawsend(sockfd,buffer,tot_len);
    
    //�ȴ�����SYN-ACK
    rawrecv(sockfd, buffer, MAX_PKT_SIZE);
    
    //����ACK
    tot_len = buildackpkt(buffer,recvacknumber,TCP_TSOPT);
    rawsend(sockfd,buffer,tot_len);
    
    return 0;

}

int rawconnrst(int sockfd)
{
    u8 buffer[MAX_PKT_SIZE];
    u16 tot_len;
    
    //ֱ��RST�ر�����
    tot_len = buildrstpkt(buffer);
    rawsend(sockfd,buffer,tot_len);
    
    return 0;
}










