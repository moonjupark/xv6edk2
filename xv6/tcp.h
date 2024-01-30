#ifndef TCP_H
#define TCP_H
#include "types.h"

void tcp_proc(uint buffer_addr);
void tcp_pkt_create(uint recv_addr,uint send_addr,uint *send_size,uint pkt_type,uint payload_size);
ushort tcp_chksum(uint tcp_addr);
void tcp_fin();

struct __attribute__((packed)) tcp_dummy {
  uchar src_ip[4];
  uchar dst_ip[4];
  uchar padding;
  uchar protocol;
  ushort tcp_len;
};
#define TCP_CODEBITS_URG 0x20
#define TCP_CODEBITS_ACK 0x10
#define TCP_CODEBITS_PSH 0x8
#define TCP_CODEBITS_RST 0x4
#define TCP_CODEBITS_SYN 0x2
#define TCP_CODEBITS_FIN 0x1
#endif
