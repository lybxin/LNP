#ifndef	_RAWOPS_H
#define	_RAWOPS_H


typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int s32;


extern u32 recvacknumber;
extern u32 recvackseq;
extern u32 recvseq;
extern u32 tcpseq;
extern u32 recvendseq;
extern u32 senddelay;
extern u32 sackblknum;
extern u16 mssval;
extern u16 tcpwz;
extern u32 tcptsopt;
extern char srcip[];
extern char dstip[];
extern u32 dsackenable;

#define TCP_TSOPT    (1<<0)
#define TCP_SACKOPT  (1<<1)
#define TCP_OFO      (1<<2)
#define TCP_DISCARD  (1<<3)
#define TCP_USER1    (1<<4)
#define TCP_TSADV    (1<<5)
#define TCP_FIN      (1<<6)
#define TCP_SYN      (1<<7)
#define TCP_ACK      (1<<8)
#define TCP_RST      (1<<9)
#define TCP_ECE      (1<<10)
#define TCP_CWR      (1<<12)


/*
# define TH_FIN	0x01
# define TH_SYN	0x02
# define TH_RST	0x04
# define TH_PUSH 0x08
# define TH_ACK	0x10
# define TH_URG	0x20

*/

#define TH_ECE	0x40
#define TH_CWR	0x80





//#define TCP_TSUPT    (1<<2)


#define MAX_PKT_SIZE (65535)
#define SERV_PORT01 SERV_PORT
#define SERV_PORT02 (10000)
#define MAX_PUREACK (100)



void initrawops(int sockfd);
int rawconnect(int sockfd);
int rawconnrst(int sockfd);
void showpkt(const unsigned char* const buffer, int buflen);
u16 builddatapkt(u8 *buffer, u32 acknumber,u16 buflen);
u16 containdata(u8 *buffer, u16 recvlen);
u16 containfin(u8 *buffer, u16 recvlen);
void updaterecvstate(u8 *buffer, u16 recvlen,u32 flag);
u16 rawrecv(int sockfd, u8 *buffer, u16 buflen);
u16 rawadvrecv(int sockfd, u8 *buffer, u16 buflen, u32 flag);
u16 rawsend(int sockfd, u8 *buffer, u16 buflen);
u16 buildackpkt(u8 *buffer, u32 acknumber, u32 flag);
void sleep_ms(int timeval);
void resetsackblk();
void appendsackblk(u32 blkbegin, u32 blkend);
int adddelaylinktail(int remaintime, u32 acknumber, u32 flag);
int popdelaylinkhead(struct timespec *timeout, u32 *acknumber, u32 *flag, unsigned char *buffer, u32 *tot_len);

int peekacknumber();
void sleep_abs_ms(struct timespec *req);
u16 rawsendnodelay(int sockfd, u8 *buffer, u16 buflen);
int del_ofo_link();
void get_adv_time(struct timespec *timeout, u32 time_ms);
u16 buildadvdatapkt(u8 *buffer, u32 acknumber,u16 buflen, u32 flag);
int rawadvconnect(int sockfd, u32 synflag, u32 synackflag);
u8 getthflag(u8 *buffer);





#endif
