#ifndef	_RAWOPS_H
#define	_RAWOPS_H

#define __BIG_ENDIAN_BITFIELD

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern u32 recvacknumber;
extern char srcip[];
extern char dstip[];

#define MAX_SEG_SIZE (65535)
#define SERV_PORT01 SERV_PORT
#define SERV_PORT02 (10000)


void initrawops();
int rawconnect(int sockfd);


/*
struct iphdr {
#if defined(__LITTLE_ENDIAN_BITFIELD)
	u8	ihl:4,
		version:4;
#elif defined (__BIG_ENDIAN_BITFIELD)
	u8	version:4,
  		ihl:4;
#else
#error	"Please fix <asm/byteorder.h>"
#endif
	u8	tos;
	u16	tot_len;
	u16	id;
	u16	frag_off;
	u8	ttl;
	u8	protocol;
	u16	check;
	u32	saddr;
	u32	daddr;
	//The options start here. 
};



struct tcphdr {
	u16	source;
	u16	dest;
	u32	seq;
	u32	ack_seq;
#if defined(__LITTLE_ENDIAN_BITFIELD)
	u16	res1:4,
		doff:4,
		fin:1,
		syn:1,
		rst:1,
		psh:1,
		ack:1,
		urg:1,
		ece:1,
		cwr:1;
#elif defined(__BIG_ENDIAN_BITFIELD)
	u16	doff:4,
		res1:4,
		cwr:1,
		ece:1,
		urg:1,
		ack:1,
		psh:1,
		rst:1,
		syn:1,
		fin:1;
#else
#error	"Adjust your <asm/byteorder.h> defines"
#endif	
	u16	window;
	u16	check;
	u16	urg_ptr;
};

*/


#endif
