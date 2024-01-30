#ifndef PKTS_HDR_H
#define PKTS_HDR_H
#include "types.h"

#define PKT_SIZE 1600
#define ETH_TYPE_ARP_HOST 0x0806
#define ETH_TYPE_ARP_NET 0x0608

struct __attribute__((packed)) eth_pkt {
//  uchar pre_amb[8];
  uchar dst_mac[6];
  uchar src_mac[6];
  uchar type[2];
};

struct __attribute__((packed)) arp_pkt {
  ushort hrd_type;
  ushort pro_type;
  uchar hrd_len;
  uchar pro_len;
  ushort op;
  uchar src_mac[6];
  uchar src_ip[4];
  uchar dst_mac[6];
  uchar dst_ip[6];
};

struct __attribute__((packed)) ipv4_pkt {
  uchar ver;
  uchar srv_type;
  ushort total_len;
  ushort id;
  ushort fragment;
  uchar ttl;
  uchar protocol;
  ushort chk_sum;
  uchar src_ip[4];
  uchar dst_ip[4];  
};

struct __attribute__((packed)) icmp_echo_pkt {
  uchar type;
  uchar code;
  ushort chk_sum;
  ushort id;
  ushort seq_num;
  uchar time_stamp[8];
  uchar data[48];
};

struct __attribute__((packed)) tcp_pkt {
  ushort src_port;
  ushort dst_port;
  uint seq_num;
  uint ack_num;
  uchar code_bits[2];
  ushort window;
  ushort chk_sum;
  ushort urgent_ptr;
};

#define ETH_TYPE_IPV4_HOST 0x0800
#define ETH_TYPE_IPV4_NET 0x0008

#endif
