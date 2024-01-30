#include "icmp.h"
#include "ipv4.h"
#include "i8254.h"
#include "pkts_hdr.h"
#include "defs.h"
#include "eth.h"

extern uchar mac_addr[6];
extern uchar my_ip[4];
extern ushort send_id;
void icmp_proc(uint buffer_addr){
  struct ipv4_pkt *ipv4_p = (struct ipv4_pkt *)(buffer_addr+sizeof(struct eth_pkt));
  struct icmp_echo_pkt *icmp_p = (struct icmp_echo_pkt *)((uint)ipv4_p + (ipv4_p->ver&0xF)*4);
  if(icmp_p->code == 0){
    if(icmp_p->type == ICMP_TYPE_ECHO_REQUEST){
      uint send_addr = (uint)kalloc();
      uint send_size = 0;
      icmp_reply_pkt_create(buffer_addr,send_addr,&send_size);
      i8254_send(send_addr,send_size);
      kfree((char *)send_addr);
    }
  }
}

void icmp_proc_req(struct icmp_echo_pkt * icmp_p){
  cprintf("ICMP ID:0x%x SEQ NUM:0x%x\n",N2H_ushort(icmp_p->id),N2H_ushort(icmp_p->seq_num));
}

void icmp_reply_pkt_create(uint recv_addr,uint send_addr,uint *send_size){
  struct eth_pkt *eth_recv = (struct eth_pkt *)(recv_addr);
  struct ipv4_pkt *ipv4_recv = (struct ipv4_pkt *)(recv_addr+sizeof(struct eth_pkt));
  struct icmp_echo_pkt *icmp_recv = (struct icmp_echo_pkt *)((uint)ipv4_recv+(ipv4_recv->ver&0xF)*4);
  struct eth_pkt *eth_send = (struct eth_pkt *)(send_addr);
  struct ipv4_pkt *ipv4_send = (struct ipv4_pkt *)(send_addr+sizeof(struct eth_pkt));
  struct icmp_echo_pkt *icmp_send = (struct icmp_echo_pkt *)((uint)ipv4_send+sizeof(struct ipv4_pkt));
  
  *send_size = sizeof(struct eth_pkt) + sizeof(struct ipv4_pkt) + sizeof(struct icmp_echo_pkt);
  memmove(eth_send->dst_mac,eth_recv->src_mac,6);
  memmove(eth_send->src_mac,mac_addr,6);
  eth_send->type[0] = 0x08;
  eth_send->type[1] = 0x00;

  ipv4_send->ver = ((0x4)<<4)+((sizeof(struct ipv4_pkt)/4)&0xF);
  ipv4_send->srv_type = 0;
  ipv4_send->total_len = H2N_ushort(sizeof(struct ipv4_pkt) + sizeof(struct icmp_echo_pkt));
  ipv4_send->id = send_id;
  send_id++;
  ipv4_send->fragment = H2N_ushort(0x4000);
  ipv4_send->ttl = 255;
  ipv4_send->protocol = 0x1;
  memmove(ipv4_send->src_ip,my_ip,4);
  memmove(ipv4_send->dst_ip,ipv4_recv->src_ip,4);
  ipv4_send->chk_sum = 0;
  ipv4_send->chk_sum = H2N_ushort(ipv4_chksum((uint)ipv4_send));

  icmp_send->type = ICMP_TYPE_ECHO_REPLY;
  icmp_send->code = 0;
  icmp_send->id = icmp_recv->id;
  icmp_send->seq_num = icmp_recv->seq_num;
  memmove(icmp_send->time_stamp,icmp_recv->time_stamp,8);
  memmove(icmp_send->data,icmp_recv->data,48);
  icmp_send->chk_sum = 0;
  icmp_send->chk_sum = H2N_ushort(icmp_chksum((uint)icmp_send));
}

ushort icmp_chksum(uint icmp_addr){
  uchar* bin = (uchar *)icmp_addr;
  uint chk_sum = 0;
  for(int i=0;i<32;i++){
    chk_sum += (bin[i*2]<<8)+bin[i*2+1];
    if(chk_sum > 0xFFFF){
      chk_sum = (chk_sum&0xFFFF)+1;
    }
  }
  return ~(chk_sum);
}
