#include "common.h"
#include "rawops.h"
#include <pthread.h>   

/*
 *未考虑系列号 ack number翻转等场景
 */
u16 clientport = SERV_PORT02;
u16 ipid = 3289;
u32 tcpseq = 8156;
//u32 inittsval = 4581;
u16 mssval = 1460;
u16 tcpwz = 4096;

//TSopt
u32 TsRecent = 0;
u32 LastAck = 0;

u32 recvtsval = 0;
u32 recvacknumber = 0;
u32 recvseq = 0;
u32 recvendseq = 0;
u32 recvackseq = 0;
u32 tcptsopt = 1;

//SACK相关信息
#define MAX_SACK_BLK 4
u32 sackblock[MAX_SACK_BLK][2]={0};
u32 startidx = 0;
u32 sackblknum = 0;
//重新修改collapseacknum 函数后validblknum应该与sackblknum一致，该变量和getsackblknum应该可以去掉了
u32 validblknum = 0;
u32 sackcollapse=1;
u32 curhighseq = 0;

//only support dsack blks before ack number
u32 dsackenable=0;
u32 dsackblk[2]={0};


#define MAX(a, b) (((s32)((a)-(b))>0)?(a):(b))
#define MIN(a, b) (((s32)((a)-(b))<0)?(a):(b))


u32 senddelay = 0;   //延迟发送报文 单位ms

char srcip[32] = {"127.0.0.1"};
char dstip[32] = {"127.0.0.1"};

/*
struct sack_blks
{
	u32 sackblock[MAX_SACK_BLK][2];
	u32 startidx;
	u32 sackblknum;
};
*/

struct ofo_node
{
    struct ofo_node* next;
	u32 startseq;
	u32 endseq;
};

struct ofo_node ofohead = {0};
struct ofo_node *ofo_link_head = &ofohead;
//struct ofo_node *ofo_link_tail = &ofohead;
u32 marksack = 0;

int insert_ofo_link(u32 startseq, u32 endseq)
{
    struct ofo_node *prev = ofo_link_head, *next = ofo_link_head->next, *p;

	if(startseq == endseq)
	{
	    return 0;
	}

	
    while(next != NULL)
    {
        printf("[insert_ofo_link]startseq:%u,endseq:%u,nstartseq:%u,nnextseq:%u,min:%u,max:%u\n",
			startseq,endseq,next->startseq,next->endseq,MIN(next->startseq, startseq),MAX(next->endseq,endseq));
        if(startseq < next->startseq || startseq == next->endseq)
        {   
            //先检测ofo报文是否可以合并
            if(endseq >= next->startseq)
            {
                //next->startseq = (next->startseq < startseq) ? next->startseq : startseq;
				next->startseq = MIN(next->startseq, startseq);
				next->endseq = MAX(next->endseq,endseq);

                //当新收的乱序包覆盖多个之前收到的乱序包的时候  清理之前收到的乱序包节点
                prev = next;
                next = prev->next;
                while(next != NULL && endseq >= next->startseq)
                {
                    prev->endseq = (prev->endseq > next->endseq) ? prev->endseq : next->endseq;
                    prev->next = next->next;
                    free(next);
                    next = prev->next;                 
                }
                
                return 1;            
            }
            //不可以合并则分配新的ofo节点
            goto alloc;
   
        }
        
        
        prev = next;
        next = next->next;
   }

alloc:
    p = malloc(sizeof(struct ofo_node));

    if(p==NULL)
    {
        return 0;
    }
    p->startseq = startseq;
    p->endseq = endseq;
    prev->next = p;
    p->next = next;
    
    return 1;

}

//仅考虑ofo队列按照startseq排列  且不包含重叠块的场景
int update_ofo_link(u32 *acknumber)
{
    struct ofo_node *p = ofo_link_head->next, *prev = ofo_link_head;
    int freenum = 0;

    printf("[update_ofo_link]acknumber:%u,recvtsval:%u,recvackseq:%u,recvseq:%u,recvendseq:%u\n",
             recvacknumber,recvtsval,recvackseq,recvseq,recvendseq);
	if(p == NULL)
	{
	    return 0;
	}
    
    while(*acknumber >= p->startseq)
    {
        if(*acknumber > p->endseq)
        {
            prev->next = p->next;
            free(p);
            freenum++;
            p = prev->next;
			if(p == NULL)
			{
			    return freenum;
			}
            continue;        
        }else
        {
            *acknumber = p->endseq;
			prev->next = p->next;
            free(p);
            freenum++; 
			return freenum;
        }
        
    }

    return freenum;
}

int del_ofo_link()
{
    struct ofo_node *p = ofo_link_head->next, *next;
    int freenum = 0;
	

	printf("[del_ofo_link]acknumber:%u,recvtsval:%u,recvackseq:%u,recvseq:%u,recvendseq:%u\n",
             recvacknumber,recvtsval,recvackseq,recvseq,recvendseq);
		
    while(p != NULL)
    {
        next = p->next;
        free(p);
        freenum++;
        p = next;
    }

	ofo_link_head->next = NULL;
    
    return freenum;

}

int find_ofo_link(u32 *startseq, u32 *endseq)
{
    struct ofo_node *p = ofo_link_head->next;
    
    while(p != NULL)
    {   
        printf("----------[find_ofo_link]error startseq:%u,endseq:%u,pstartseq:%u,pendseq:%u---------------------\n", 
			*startseq, *endseq,p->startseq,p->endseq);
        if( (p->startseq <= *startseq) && (p->endseq >= *endseq) )
        {
            *startseq = p->startseq;
            *endseq = p->endseq;
            return 1;
        }
        p = p->next;
    }
    
    printf("----------[find_ofo_link]error startseq:%u,endseq:%u---------------------\n", *startseq, *endseq);
    return 0;

}



struct delay_node
{
    struct delay_node* next;
	//int remaintime;
	int acknumber;
	u32 flag;
	struct timespec timeout;
	unsigned char buffer[MAX_PUREACK];
	u32 tot_len;
};



struct delay_node headnode = {0};

struct delay_node *delay_link_head = &headnode;
struct delay_node *delay_link_tail = &headnode;
pthread_mutex_t delaylinkmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  delaylinkcond = PTHREAD_COND_INITIALIZER;

void get_adv_time(struct timespec *timeout, u32 time_ms)
{
    u32 timeout_s,timeout_ms;
	struct timespec req;

	timeout_s = time_ms/1000;
	timeout_ms = time_ms - timeout_s*1000;
	clock_gettime(CLOCK_MONOTONIC, &req);

	if( (timeout_ms*1000*1000 + req.tv_nsec) < (1000*1000*1000) )
	{
        timeout->tv_nsec = timeout_ms*1000*1000 + req.tv_nsec;
		timeout->tv_sec = req.tv_sec + timeout_s;
	}else
	{
        timeout->tv_nsec = timeout_ms*1000*1000 + req.tv_nsec - (1000*1000*1000);
		timeout->tv_sec = req.tv_sec + timeout_s + 1;
	}
	

}

//remain's resolution is ms
int adddelaylinktail(int remaintime, u32 acknumber, u32 flag)
{
    struct delay_node *p;
	struct timespec req;
    
	p = malloc(sizeof(struct delay_node));
	memset(p, 0, sizeof(struct delay_node));

	if(p==NULL)
	{
        return 0;
	}

	clock_gettime(CLOCK_MONOTONIC, &req);

	if( (remaintime*1000*1000 + req.tv_nsec) < (1000*1000*1000) )
	{
        p->timeout.tv_nsec = remaintime*1000*1000 + req.tv_nsec;
		p->timeout.tv_sec = req.tv_sec;
	}else
	{
        p->timeout.tv_nsec = remaintime*1000*1000 + req.tv_nsec - (1000*1000*1000);
		p->timeout.tv_sec = req.tv_sec + 1;
	}
	printf("[adddelaylinktail]:acknumber:%u,remaintime:%u\n",acknumber,remaintime);
	p->next = NULL;
	//p->remaintime = remaintime;
	p->acknumber = acknumber;
	p->flag = flag;
	p->tot_len = buildackpkt(p->buffer,acknumber,flag);
	//add delay === already send
	LastAck = acknumber;

	pthread_mutex_lock(&delaylinkmutex);
	delay_link_tail->next = p;
	delay_link_tail = p;

	if(delay_link_head->next == delay_link_tail)
	{
	    pthread_cond_signal(&delaylinkcond); 
	}
	pthread_mutex_unlock(&delaylinkmutex);


	return 1;

}

int popdelaylinkhead(struct timespec *timeout, u32 *acknumber, u32 *flag, unsigned char *buffer, u32 *tot_len)
{
    struct delay_node *p;

	pthread_mutex_lock(&delaylinkmutex);

	while(delay_link_head->next == NULL)
	{
         pthread_cond_wait(&delaylinkcond, &delaylinkmutex);
	}

	if(delay_link_head->next != NULL)
	{
	    p = delay_link_head->next;
		delay_link_head->next = p->next;
		timeout->tv_sec = p->timeout.tv_sec;
		timeout->tv_nsec = p->timeout.tv_nsec;
		*acknumber = p->acknumber;
		*flag = p->flag;
		*tot_len = p->tot_len;
		memmove(buffer,p->buffer,100);
		if(delay_link_tail == p)
		{
		    delay_link_tail = delay_link_head;
		}
		free(p);
		pthread_mutex_unlock(&delaylinkmutex);
		
		return 1;
	}
	pthread_mutex_unlock(&delaylinkmutex);
	return 0;

}


int peekacknumber()
{
    struct delay_node *p;
	u32 acknumber = -1;

	pthread_mutex_lock(&delaylinkmutex);
	if(delay_link_head->next != NULL)
	{
	    p = delay_link_head->next;
		delay_link_head->next = p->next;
		acknumber = p->acknumber;
		free(p);
		pthread_mutex_unlock(&delaylinkmutex);
		return acknumber;
	}
	pthread_mutex_unlock(&delaylinkmutex);
	return recvacknumber;

}



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

struct thtsopt
{
        
    u16 nop1:8,
        nop2:8;
        
    u16 tskind:8,
        tslen:8;
    u32 tsval;
    u32 tsecr;  
       
};

//调试使用  与wireshark输出格式相同 方便对比
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
    
    //初始化ipid和tcpseq
    srandom(time(NULL));
    ipid = (u16)random();
    tcpseq = (u16)random();
    startidx = 0;
    sackblknum = 0;
	curhighseq = 0;
    
    printf("[initrawops] ipid:%u,tcpseq:%u\n",ipid,tcpseq);
    
    Setsockopt(sockfd,IPPROTO_IP, IP_HDRINCL, &optval, sizeof(optval));
    
    //绑定本地接收地址  只接收发往srcip的报文
    memset(&bindaddr,0,sizeof(bindaddr));
    bindaddr.sin_family = AF_INET;
    bindaddr.sin_addr.s_addr = inet_addr(srcip);
    //bindaddr.sin_port = htons(SERV_PORT01);
    Bind(sockfd,(SA*)&bindaddr,sizeof(bindaddr));
    
    //绑定本地发送地址  使用send直接发送报文
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    servaddr.sin_addr.s_addr = inet_addr(dstip);
    //inet_pton(AF_INET,argv[1],&servaddr.sin_addr);
    Connect(sockfd,(SA*)&servaddr,sizeof(servaddr));
}

//获取TSopt中的tsval  每1ms自增1
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

//注意checksum和tot_len
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
    
    iph->check = htons(csum ((unsigned short *) iph, ntohs(iph->tot_len)));
}

//头长doff check  确保optlen四字节对齐
void filltcphdr(struct tcphdr *th, int syn, int ack, u32 seq, u32 acknumber, u16 optlen)
{
	
    //TCP Header
    th->source = htons (clientport);
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
    th->check = 0;                 //计算checksum前 需要先将check设置为0
    th->urg_ptr = 0;
}

//更新tcp checksum
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

//填充SYN报文TCP选项
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

//建立SYN报文 外层确保buffer len足够长
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
void mergesackblks()
{
    u32 startseq, endseq;
    u8 idx,blknum = sackblknum;
    //尝试collapse
    if(sackcollapse == 1)
    {
        //获取新收到的ofo报文的起始系列号
	    idx = (startidx + MAX_SACK_BLK - 1)%MAX_SACK_BLK;
	    startseq = sackblock[idx][0];
	    endseq = sackblock[idx][1];
	    
	    //合并之前收到的sack块
	    idx = (idx + MAX_SACK_BLK - 1)%MAX_SACK_BLK;
	    blknum--;
	    while(blknum > 0)
	    {
	        if( (startseq <= sackblock[idx][0]) && (endseq >= sackblock[idx][1]))
	        {
	            //在collapseacknum函数会中去掉无效的sack blk
	            sackblock[idx][0] = 0;
	            sackblock[idx][1] = 0;
	        }
	        idx = (idx + MAX_SACK_BLK - 1)%MAX_SACK_BLK;
	        blknum--;
	    }
	    
    } 
	    
}

void collapseacknum(u32 *acknumber, u32 check)
{
    u8 idx,blknum = sackblknum;
    u32 validblks[MAX_SACK_BLK][2]={0}, validnum = 0, flag = 0;
    
    //尝试collapse
    if(sackcollapse == 1)
    {
	    idx = (startidx + MAX_SACK_BLK - 1)%MAX_SACK_BLK;			               
		while(blknum>0)
		{
			if( (*acknumber >= sackblock[idx][0] && (sackblock[idx][0] != 0))  )
			{
			    *acknumber = MAX(sackblock[idx][1],*acknumber);

				sackblock[idx][0] = 0;
				sackblock[idx][1] = 0;
			    flag = 1;
			    
			    idx = (startidx + MAX_SACK_BLK - 1)%MAX_SACK_BLK ;	
			    blknum = sackblknum;
			}else
			{
			    idx = (idx + MAX_SACK_BLK - 1)%MAX_SACK_BLK ;
			    --blknum;		
			}

		} 
		
		//没有发生sack collapse 直接退出
		if(!flag && !check)
		{
		    return;
		}

		idx = (startidx + MAX_SACK_BLK - 1)%MAX_SACK_BLK;	
		blknum = sackblknum;

		while( blknum>0)
		{
		    //以0判断有效值  不考虑系列号碰到0翻转等场景
		    if((sackblock[idx][0] != 0) && (sackblock[idx][1] != 0))
		    {
		        validblks[validnum][0] = sackblock[idx][0];
		        validblks[validnum][1] = sackblock[idx][1];
		        validnum++;		    
		    }
		    idx = (idx + MAX_SACK_BLK - 1)%MAX_SACK_BLK ;
		    --blknum;
		}
		
		resetsackblk();
		while(validnum > 0)
		{
		    validnum--;
		    appendsackblk(validblks[validnum][0], validblks[validnum][1]);
		}

		
    }  
}

u8 getsackblknum(u32 flag)
{
    u8 blknum = sackblknum;
	if(dsackenable&&dsackblk[0])
	{
        blknum++;
	}

	if(flag&TCP_TSOPT)
	{
		blknum = MIN(blknum,3);
	}else
	{
	    blknum = MIN(blknum,4);
	}
	
    return blknum;
}

//填充ACK报文和普通数据报文的TCP选项 tsecr 需要recv更新
//添加SACK支持的时候 需要注意sizeof(struct thtsopt)
void fillnormalopts(struct tcphdr* th, u32 flag, u32 acknumber)
{
    struct thtsopt *tsopt;
    u32 *ptr,idx;
	u8 blknum = getsackblknum(flag);

	if(flag&TCP_TSOPT)
	{
	    tsopt = (struct thtsopt*)((u8*)th + 20);
    
	    tsopt->nop1 = 1;
	    tsopt->nop2 = 1;
	    
	    tsopt->tskind = 8;
	    tsopt->tslen = 10;
	    tsopt->tsval = htonl(gettsval());
		if(flag&TCP_TSADV)
		{
            tsopt->tsecr = htonl(TsRecent);
		}else
        {
            tsopt->tsecr = htonl(recvtsval);
	    }
	    
	    ptr = (u32*) ((u8*)th + 20 + sizeof(struct thtsopt));
		
	}else
	{
	    ptr = (u32*) ((u8*)th + 20);
		
	}
    
	printf("[fillnormalopts]d sackenable:%u,d sack:%u,%u,blknum:%u,flag:%u\n",dsackenable,dsackblk[0],dsackblk[1],blknum,flag);
    if((flag&TCP_SACKOPT) && (blknum > 0))
    {

        *ptr++ = htonl((1  << 24) |
		               (1  << 16) |
			           (5 <<  8)  |
			           (2 + blknum *8) );
		idx = (startidx + MAX_SACK_BLK - 1)%MAX_SACK_BLK;	
		if(dsackenable&&dsackblk[0])
	    {
	        *ptr++ = htonl(dsackblk[0]);
		    *ptr++ = htonl(dsackblk[1]);
			blknum--;
		}
			               
		while(blknum>0)
		{
		    if(acknumber < sackblock[idx][0])
		    {
		    	*ptr++ = htonl(sackblock[idx][0]);
		        *ptr++ = htonl(sackblock[idx][1]);
		    }
		    

			idx = (idx + MAX_SACK_BLK - 1)%MAX_SACK_BLK ;
		    --blknum;
		}	                     
        
    }
	//dsack only valid for one time	
	dsackblk[0]=0;
	dsackblk[1]=0;

}


//外层确保buffer len足够长
u16 buildackpkt(u8 *buffer, u32 acknumber, u32 flag)
{
    struct iphdr *iph;
    struct tcphdr *th;
    u16 tot_len, optlen = 0;
	if(!tcptsopt)
	{
        flag&= ~TCP_TSOPT;
	}
    
    if(flag&TCP_TSOPT)
    {
        optlen += sizeof(struct thtsopt);
    }

	collapseacknum(&acknumber,0);
    if(flag&TCP_SACKOPT && getsackblknum(flag) > 0)
    {
        optlen += getsackblknum(flag) * 8 + 4;
    }
    
    iph = (struct iphdr*)buffer;
    
    tot_len = sizeof(struct iphdr)+sizeof(struct tcphdr)+ optlen;
    
    filliphdr(iph, tot_len);
    
    th = (struct tcphdr*)(buffer + iph->ihl * 4);
    
    filltcphdr(th, 0, 1, tcpseq, acknumber, optlen);
    
    fillnormalopts(th, flag, acknumber);
    
    printf("[buildackpkt] optlen:%u,tot_len:%u,Ack:%u,sacknum:%u,sackblknum:%u\n",optlen,tot_len,acknumber,getsackblknum(flag),sackblknum);
    
    updatetcphdr(th, optlen);
        
    return tot_len;
}

u16 buildadvdatapkt(u8 *buffer, u32 acknumber,u16 buflen, u32 flag)
{
    struct iphdr *iph;
    struct tcphdr *th;
    u16 tot_len,hdrlen,optlen;;

	if(flag|TCP_TSOPT)
	{
		optlen=0;
	}else
	{
	    optlen = sizeof(struct thtsopt);
	}
    
    hdrlen = sizeof(struct iphdr)+sizeof(struct tcphdr)+optlen;
    
    memmove((buffer + hdrlen), buffer, buflen);
    
    iph = (struct iphdr*)buffer;
    
    tot_len = hdrlen + buflen;
    
    filliphdr(iph, tot_len);
    
    th = (struct tcphdr*)(buffer + iph->ihl * 4);
    
    filltcphdr(th, 0, 1, tcpseq, acknumber, optlen);
    
    fillnormalopts(th,flag,acknumber);
    
    updatetcphdr(th, ( optlen + buflen ) );
    
    return tot_len;
}


//外层确保buffer len足够长  返回添加hdr后的tot_len 
//should refactor flag to arguments
u16 builddatapkt(u8 *buffer, u32 acknumber,u16 buflen)
{

    u16 tot_len;
	u32 flag = 0;

	if(!tcptsopt)
	{
        flag&=~TCP_TSOPT;
	}else
	{
	    flag |= TCP_TSOPT;
	}

	tot_len = buildadvdatapkt(buffer, acknumber,buflen, flag);
    
    
    return tot_len;
}

//外层确保buffer len足够长
u16 buildrstpkt(u8 *buffer)
{
    struct iphdr *iph;
    struct tcphdr *th;
    u16 tot_len,optlen;
	u32 flag=0;

	if(!tcptsopt)
	{
        flag&=~TCP_TSOPT;
		optlen=0;
	}else
	{
	    flag |= TCP_TSOPT;
	    optlen = sizeof(struct thtsopt);

	}
    
    iph = (struct iphdr*)buffer;
    
    tot_len = sizeof(struct iphdr)+sizeof(struct tcphdr)+ optlen;
    
    filliphdr(iph, tot_len);
    
    th = (struct tcphdr*)(buffer + iph->ihl * 4);
    
    filltcphdr(th, 0, 1, tcpseq, recvacknumber, optlen);
    
    th->rst = 1;
    
    fillnormalopts(th,flag,0);
    
    updatetcphdr(th, optlen );
    
    return tot_len;
}


void updaterecvstate(u8 *buffer, u16 recvlen,u32 flag)
{
    struct iphdr *iph;
    struct tcphdr *th;
    u16 datalen;
    u8 *options, *optsend;
    //u32 endseq;
    
    iph = (struct iphdr*)buffer;
    th = (struct tcphdr*)(buffer + iph->ihl * 4);
    
    datalen = recvlen - iph->ihl*4 - th->doff*4;
    
    recvendseq = ntohl(th->seq) + th->syn + th->fin + datalen;

	recvackseq = ntohl(th->ack_seq);
	recvseq = ntohl(th->seq);
	//mark dsack info
	if(recvseq<recvacknumber)
	{
	    dsackblk[0]=recvseq;
		dsackblk[1]=MIN(recvendseq,recvacknumber);
		printf("[updaterecvstate]dsack:%u,%u\n",dsackblk[0],dsackblk[1]);
	}

    if(marksack || (flag & TCP_OFO)  || (flag & TCP_SACKOPT))
    {
        if(recvacknumber >= ntohl(th->seq))
        {
            recvacknumber = MAX(recvendseq,recvacknumber);
            update_ofo_link(&recvacknumber);
        }else
        {
            insert_ofo_link(ntohl(th->seq), recvendseq);
        }
    
    }else
    {
        recvacknumber = ntohl(th->seq) + th->syn + th->fin + datalen;
    }
	
   
    if(flag & TCP_SACKOPT)
    {
        
        if( (recvacknumber < ntohl(th->seq)) && (recvendseq > ntohl(th->seq)) )
        {
            sackblock[startidx][0] = ntohl(th->seq);
	        sackblock[startidx][1] = recvendseq;
		    find_ofo_link(&sackblock[startidx][0], &sackblock[startidx][1]);
		    startidx = (startidx +1) % MAX_SACK_BLK;
		    sackblknum++;
		    //调用完mergesackblks后需要调用collapseacknum 去掉无效的sack blk
		    mergesackblks();
        
        }
        
        collapseacknum(&recvacknumber,1);    
    }

        
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
		   if(LastAck==0)
		   {
               TsRecent = recvtsval;
		   }
		   if(recvseq == LastAck)
		   {
		       TsRecent = recvtsval;
		   }
           break;
       }
       
       //other options
       options++;
       options = options + (*options) - 1;
    }
    
    printf("[updaterecvstate]seq:%u,syn:%u,fin:%u,acknumber:%u,recvtsval:%u,\n\
		--------recvackseq:%u,recvseq:%u,recvendseq:%u,LastAck:%u,TsRecent:%u\n",
             ntohl(th->seq),th->syn,th->fin,recvacknumber,recvtsval,recvackseq,recvseq,recvendseq,LastAck,TsRecent);
    
}

//判断目的端口是否为要处理的端口
u32 pktforme(u8 *buffer)
{    
    struct iphdr *iph;
    struct tcphdr *th;
    
    iph = (struct iphdr*)buffer;
    th = (struct tcphdr*)(buffer + iph->ihl * 4);
    
    return ( th->dest == htons (clientport) );
}
u16 containdata(u8 *buffer, u16 recvlen)
{
    struct iphdr *iph;
    struct tcphdr *th;
	u16 datalen;

    iph = (struct iphdr*)buffer;
    th = (struct tcphdr*)(buffer + iph->ihl * 4);

	//caution:ihl doff is bigger endian
    datalen = recvlen - iph->ihl*4 - th->doff*4;
    datalen = th->syn + th->fin + datalen;

	return datalen;
}

u16 rawadvrecv(int sockfd, u8 *buffer, u16 buflen, u32 flag)
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
        
        //bind操作只能过滤ip地址  此处进一步过滤端口
        if(pktforme(buffer))
            break;
    }
               
    printf("\n[rawrecv]before update rcv state\n");                  
    showpkt((u8*)buffer,60);

	if(flag&TCP_DISCARD)
	{
        return recvlen;
	}
                        
    updaterecvstate(buffer, recvlen, flag);
                        
    return recvlen;


}



u16 rawrecv(int sockfd, u8 *buffer, u16 buflen)
{

                        
    return rawadvrecv(sockfd, buffer, buflen, 0);
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

//外层确保sendlen有效
void updatesendstate(u8 *buffer, u16 sendlen)
{
    struct iphdr *iph;
    struct tcphdr *th;
    u16 datalen;
    
    iph = (struct iphdr*)buffer;
    th = (struct tcphdr*)(buffer + iph->ihl * 4);
         
    datalen = sendlen - iph->ihl*4 - th->doff*4;
    
    //更新下一个pkt的seq
    tcpseq = ntohl(th->seq) + th->syn + th->fin + datalen;
	
    //更新ipid
    ipid++;
}

void sleep_ms(int timeval)
{
    struct timespec req;
    req.tv_sec = timeval/1000;
    req.tv_nsec = (timeval%1000) * 1000000;
    clock_nanosleep(CLOCK_MONOTONIC,0,&req,NULL);

}

void sleep_abs_ms(struct timespec *req)
{
    clock_nanosleep(CLOCK_MONOTONIC,TIMER_ABSTIME,req,NULL);
}

u16 rawsendnodelay(int sockfd, u8 *buffer, u16 buflen)
{
    u16 sendlen;
    
    
    sendlen = Send(sockfd, buffer, buflen, 0);
    
    //printf("\n[rawsend]before updatesendstate\n");
    //showpkt((u8*)buffer,60);
                        
    updatesendstate(buffer, sendlen);
                        
    return sendlen;
}


u16 rawsend(int sockfd, u8 *buffer, u16 buflen)
{    
    
    if(senddelay > 0)
    {
        sleep_ms(senddelay);
    }
                    
    return rawsendnodelay(sockfd, buffer, buflen);
}



//返回错误需要处理
int rawconnect(int sockfd)
{
    u8 buffer[MAX_PKT_SIZE];
    u16 tot_len;
    
    //发送SYN
    tot_len = buildsynpkt(buffer);
    rawsend(sockfd,buffer,tot_len);
    
    //等待接收SYN-ACK
    rawrecv(sockfd, buffer, MAX_PKT_SIZE);
    
    //发送ACK  init LastAck
    LastAck = recvacknumber;
    tot_len = buildackpkt(buffer,recvacknumber,TCP_TSOPT);
    rawsend(sockfd,buffer,tot_len);
    
    return 0;

}

int rawconnrst(int sockfd)
{
    u8 buffer[MAX_PKT_SIZE];
    u16 tot_len;
    
    //直接RST关闭连接
    tot_len = buildrstpkt(buffer);
    rawsend(sockfd,buffer,tot_len);
    
    return 0;
}


void resetsackblk()
{
    startidx = 0;
    sackblknum = 0;
}

void appendsackblk(u32 blkbegin, u32 blkend)
{
    
    sackblock[startidx][0] = blkbegin;
    sackblock[startidx][1] = blkend;

	printf("sack block info begin:%u,end:%u\n",blkbegin,blkend);
    startidx = (startidx +1) % MAX_SACK_BLK;
    sackblknum++;
}







