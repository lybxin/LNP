#include "tcp_private.h"
#include <stdio.h>

void printftcpinfo(struct tcp_info_user *info)
{
    printf("---------------------------------------------------------------------------------------------------\n");
    printf("tcpi_segs_in:%u,tcpi_segs_out:%u\n",info->tcpi_segs_in,info->tcpi_segs_out);
    
    printf("tcpi_state:%u,tcpi_ca_state:%u,tcpi_retransmits:%u,tcpi_probes:%u,tcpi_backoff:%u,tcpi_options:%u\n",
            info->tcpi_state,info->tcpi_ca_state,info->tcpi_retransmits,
            info->tcpi_probes,info->tcpi_backoff,info->tcpi_options);
            
    printf("tcpi_rto:%u,tcpi_rtt:%u,tcpi_rttvar:%u,mdev_max_us:%u,rttvar_us:%u,rtt_seq:%u,tcpi_ato:%u\n",
            info->tcpi_rto,info->tcpi_rtt,info->tcpi_rttvar,info->mdev_max_us,
            info->rttvar_us,info->rtt_seq,info->tcpi_ato);
            
    printf("tcpi_snd_wscale:%u,tcpi_rcv_wscale:%u,tcpi_snd_mss:%u,tcpi_rcv_mss:%u\n",
            info->tcpi_snd_wscale,info->tcpi_rcv_wscale,info->tcpi_snd_mss,info->tcpi_rcv_mss);
            
    printf("tcpi_unacked(pkts_out):%u,tcpi_sacked:%u,tcpi_lost:%u,tcpi_retrans:%u,tcpi_fackets:%u\n",
            info->tcpi_unacked,info->tcpi_sacked,info->tcpi_lost,info->tcpi_retrans,info->tcpi_fackets);
            
    printf("tcpi_last_data_sent:%u,tcpi_last_ack_sent:%u,tcpi_last_data_recv:%u,tcpi_last_ack_recv:%u\n",
            info->tcpi_last_data_sent,info->tcpi_last_ack_sent,
            info->tcpi_last_data_recv,info->tcpi_last_ack_recv);
    
    printf("tcpi_pmtu:%u,tcpi_rcv_ssthresh:%u,tcpi_snd_ssthresh:%u,tcpi_snd_cwnd:%u,tcpi_advmss:%u\n",
            info->tcpi_pmtu,info->tcpi_rcv_ssthresh,info->tcpi_snd_ssthresh,
            info->tcpi_snd_cwnd,info->tcpi_advmss);
    
    printf("tcpi_reordering:%u,tcpi_rcv_rtt:%u,tcpi_rcv_space:%u,tcpi_total_retrans:%u\n",
            info->tcpi_reordering,info->tcpi_rcv_rtt,info->tcpi_rcv_space,info->tcpi_total_retrans);
    
    printf("tcpi_pacing_rate:%lu,tcpi_max_pacing_rate:%lu,tcpi_bytes_acked:%lu,tcpi_bytes_received:%lu\n",
            info->tcpi_pacing_rate,info->tcpi_max_pacing_rate,
            info->tcpi_bytes_acked,info->tcpi_bytes_received);
    
    printf("tcp_header_len:%u,gso_segs:%u,pred_flags:%u,rcv_nxt:%u,copied_seq:%u,rcv_wup:%u,snd_nxt:%u\n",
            info->tcp_header_len,info->gso_segs,info->pred_flags,
            info->rcv_nxt,info->copied_seq,info->rcv_wup,info->snd_nxt);
    
    printf("snd_una:%u,snd_wl1:%u,snd_wnd:%u,max_window:%u,snd_cwnd_cnt:%u,snd_cwnd_clamp:%u,snd_cwnd_used:%u\n",
            info->snd_una,info->snd_wl1,info->snd_wnd,info->max_window,
            info->snd_cwnd_cnt,info->snd_cwnd_clamp,info->snd_cwnd_used);
    
    printf("snd_cwnd_stamp:%u,rcv_wnd:%u,write_seq:%u,pushed_seq:%u,prior_ssthresh:%u,high_seq:%u\n",
            info->snd_cwnd_stamp,info->rcv_wnd,info->write_seq,info->pushed_seq,
            info->prior_ssthresh,info->high_seq);
   
}
