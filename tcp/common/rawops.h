#ifndef	_RAWOPS_H
#define	_RAWOPS_H

#define __BIG_ENDIAN_BITFIELD

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern u32 recvacknumber;
extern u16 tcpseq;
extern u32 senddelay;
extern char srcip[];
extern char dstip[];

#define TCP_TSOPT  (1<<0)

#define MAX_PKT_SIZE (65535)
#define SERV_PORT01 SERV_PORT
#define SERV_PORT02 (10000)


void initrawops(int sockfd);
int rawconnect(int sockfd);
int rawconnrst(int sockfd);
void showpkt(const unsigned char* const buffer, int buflen);
u16 builddatapkt(u8 *buffer, u32 acknumber,u16 buflen);
u16 rawrecv(int sockfd, u8 *buffer, u16 buflen);
u16 rawsend(int sockfd, u8 *buffer, u16 buflen);
u16 buildackpkt(u8 *buffer, u32 acknumber, u32 flag);
void sleep_ms(int timeval);


#endif
