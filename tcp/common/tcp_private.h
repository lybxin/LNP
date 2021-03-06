#ifndef	_TCP_PRIVATE_H
#define	_TCP_PRIVATE_H
#include <netinet/tcp.h>
#include <stdio.h>


//自定义TCP_INFO响应数据结构
struct tcp_info_user
{
	u_int8_t	tcpi_state;
	u_int8_t	tcpi_ca_state;
	u_int8_t	tcpi_retransmits;
	u_int8_t	tcpi_probes;
	u_int8_t	tcpi_backoff;
	u_int8_t	tcpi_options;
	u_int8_t	tcpi_snd_wscale : 4, tcpi_rcv_wscale : 4;

	u_int32_t	tcpi_rto;
	u_int32_t	tcpi_ato;
	u_int32_t	tcpi_snd_mss;
	u_int32_t	tcpi_rcv_mss;

	u_int32_t	tcpi_unacked;
	u_int32_t	tcpi_sacked;
	u_int32_t	tcpi_lost;
	u_int32_t	tcpi_retrans;
	u_int32_t	tcpi_fackets;

	/* Times. */
	u_int32_t	tcpi_last_data_sent;
	u_int32_t	tcpi_last_ack_sent;	/* Not remembered, sorry. */

	u_int32_t	tcpi_last_data_recv;
	u_int32_t	tcpi_last_ack_recv;

	/* Metrics. */
	u_int32_t	tcpi_pmtu;
	u_int32_t	tcpi_rcv_ssthresh;
	u_int32_t	tcpi_rtt;
	u_int32_t	tcpi_rttvar;
	u_int32_t	tcpi_snd_ssthresh;
	u_int32_t	tcpi_snd_cwnd;
	u_int32_t	tcpi_advmss;
	u_int32_t	tcpi_reordering;

	u_int32_t	tcpi_rcv_rtt;
	u_int32_t	tcpi_rcv_space;

	u_int32_t	tcpi_total_retrans;

	u_int64_t	tcpi_pacing_rate;
	u_int64_t	tcpi_max_pacing_rate;
	u_int64_t	tcpi_bytes_acked;    /* RFC4898 tcpEStatsAppHCThruOctetsAcked */
	u_int64_t	tcpi_bytes_received; /* RFC4898 tcpEStatsAppHCThruOctetsReceived */
	u_int32_t	tcpi_segs_out;	     /* RFC4898 tcpEStatsPerfSegsOut */
	u_int32_t	tcpi_segs_in;	     /* RFC4898 tcpEStatsPerfSegsIn */

    //user defined
	u_int16_t	tcp_header_len;	/* Bytes of tcp header to send		*/
	u_int16_t	gso_segs;	/* Max number of segs per GSO packet	*/


	u_int32_t	pred_flags;

	u_int32_t	rcv_nxt;	/* What we want to receive next 	*/
	u_int32_t	copied_seq;	/* Head of yet unread data		*/
	u_int32_t	rcv_wup;	/* rcv_nxt on last window update sent	*/
	u_int32_t	snd_nxt;	/* Next sequence we send		*/
	u_int32_t	snd_una;	/* First byte we want an ack for	*/

	//u_int32_t	rcv_tstamp;	/* timestamp of last received ACK (for keepalives) */
	u_int32_t	snd_wl1;	/* Sequence for window update		*/
	u_int32_t	snd_wnd;	/* The window we expect to receive	*/
	u_int32_t	max_window;	/* Maximal window ever seen from peer	*/
	//u_int32_t	mss_cache;	/* Cached effective mss, not including SACKS */
	u_int32_t	mdev_max_us;	/* maximal mdev for the last rtt period	*/
	u_int32_t	rttvar_us;	/* smoothed mdev_max			*/
	u_int32_t	rtt_seq;	/* sequence number to update rttvar	*/

	u_int32_t	snd_cwnd_cnt;	/* Linear increase counter		*/
	u_int32_t	snd_cwnd_clamp; /* Do not allow snd_cwnd to grow above this */
	u_int32_t	snd_cwnd_used;
	u_int32_t	snd_cwnd_stamp;


	u_int32_t	rcv_wnd;	/* Current receiver window		*/
	u_int32_t	write_seq;	/* Tail(+1) of data held in tcp send buffer */

	u_int32_t	pushed_seq;	/* Last pushed seq, required to talk to windows */
	u_int32_t	prior_ssthresh; /* ssthresh saved at recovery start	*/
	u_int32_t	high_seq;	/* snd_nxt at onset of congestion	*/

	//user define 2
	
	u_int32_t   probe_size;


    u_int8_t	nonagle     : 4,/* Disable Nagle algorithm?             */
		    thin_lto    : 1,/* Use linear timeouts for thin streams */
		    thin_dupack : 1,/* Fast retransmit on first dupack      */
		    repair      : 1,
		    frto        : 1;/* F-RTO (RFC5682) activated in CA_Loss */
    u_int8_t    rack_advanced;
	u_int8_t    rack_reord;
	u_int8_t    icsk_pending;


    u_int32_t	retrans_stamp;	/* Timestamp of the last retransmit,
				 * also used in SYN-SENT to remember stamp of
				 * the first SYN. */
    u_int32_t	undo_marker;	/* snd_una upon a new recovery episode. */

	u_int32_t   undo_retrans;	

	u_int32_t   tlp_high_seq;
	
	
	//user define 3
    int32_t   sk_rcvbuf;
	int32_t   sk_rmem_alloc_i;
	int32_t   sk_sndbuf;
	int32_t   sk_wmem_alloc;
	int32_t   sk_omem_alloc;
	int32_t   sk_wmem_queued;

	u_int32_t   window_clamp;

	int32_t   space;
	u_int32_t   seq;
	u_int16_t   skblen;
	u_int16_t   sslen;
	
	//user define 4
    u_int8_t   quick;
    u_int8_t   pingpong;
	u_int8_t   pending;
	u_int8_t   is_cwnd_limited:1;
	u_int8_t   rsv0:7;
	
	
	u_int32_t  lrcvtime;
	int32_t  sk_forward_alloc;

    u_int32_t  max_packets_out;
	long   memory_allocated;

	u_int8_t   caname[8];

	u_int32_t  max_packets_seq;

	u_int32_t retransmit_high;
    u_int32_t highest_sack_seq;
	u_int32_t prr_delivered;
	u_int32_t prr_out;
	
};

void fprintftcpinfo(FILE *stream, struct tcp_info_user *info);

void printftcpinfo(struct tcp_info_user *info);


#endif

