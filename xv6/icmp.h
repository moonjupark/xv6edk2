#ifndef ICMP_H
#define ICMP_H
#include "pkts_hdr.h"

#define ICMP_TYPE_ECHO_REQUEST 0x8
#define ICMP_TYPE_ECHO_REPLY 0x0

void icmp_proc(uint buffer_addr);
void icmp_proc_req(struct icmp_echo_pkt * icmp_p);
void icmp_reply_pkt_create(uint recv_addr,uint send_addr,uint *send_size);
ushort icmp_chksum(uint icmp_addr);
#endif
