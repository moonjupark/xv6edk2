#include "tcp.h"
#include "eth.h"
#include "ipv4.h"
#include "pkts_hdr.h"
#include "defs.h"
#include "i8254.h"
#include "http.h"

uint seq_num = 0;
extern ushort send_id;
extern uchar mac_addr[6];
extern uchar my_ip[4];
int fin_flag = 0;

void tcp_proc(uint buffer_addr){
  struct ipv4_pkt *ipv4_p = (struct ipv4_pkt *)(buffer_addr + sizeof(struct eth_pkt));
  struct tcp_pkt *tcp_p = (struct tcp_pkt *)((uint)ipv4_p + (ipv4_p->ver&0xF)*4);
  char *payload = (char *)((uint)tcp_p + 20);
  uint send_addr = (uint)kalloc();
  uint send_size = 0;
  if(tcp_p->code_bits[1]&TCP_CODEBITS_SYN){
    tcp_pkt_create(buffer_addr,send_addr,&send_size,TCP_CODEBITS_ACK | TCP_CODEBITS_SYN,0);
    i8254_send(send_addr,send_size);
    seq_num++;
  }else if(tcp_p->code_bits[1] == (TCP_CODEBITS_PSH | TCP_CODEBITS_ACK)){
    if(memcmp(payload,"GET",3)){
      cprintf("ACK PSH\n");
      tcp_pkt_create(buffer_addr,send_addr,&send_size,TCP_CODEBITS_ACK,0);
      i8254_send(send_addr,send_size);
      uint send_payload = (send_addr + sizeof(struct eth_pkt) + sizeof(struct ipv4_pkt) + sizeof(struct tcp_pkt));
      uint payload_size;
      http_proc(0,0,send_payload,&payload_size);
      tcp_pkt_create(buffer_addr,send_addr,&send_size,(TCP_CODEBITS_ACK|TCP_CODEBITS_PSH),payload_size);
    }else{
     tcp_pkt_create(buffer_addr,send_addr,&send_size,TCP_CODEBITS_ACK,0);
     i8254_send(send_addr,send_size);
      uint send_payload = (send_addr + sizeof(struct eth_pkt) + sizeof(struct ipv4_pkt) + sizeof(struct tcp_pkt));
      uint payload_size;
      http_proc(0,0,send_payload,&payload_size);
      tcp_pkt_create(buffer_addr,send_addr,&send_size,(TCP_CODEBITS_ACK|TCP_CODEBITS_PSH),payload_size);
    }
    i8254_send(send_addr,send_size);
    seq_num++;
  }else if(tcp_p->code_bits[1] == TCP_CODEBITS_ACK){
    if(fin_flag == 1){
      tcp_pkt_create(buffer_addr,send_addr,&send_size,TCP_CODEBITS_FIN,0);
      i8254_send(send_addr,send_size);
      fin_flag = 0;
    }
  }
  kfree((char *)send_addr);
}

void tcp_pkt_create(uint recv_addr,uint send_addr,uint *send_size,uint pkt_type,uint payload_size){
  struct eth_pkt *eth_recv = (struct eth_pkt *)(recv_addr);
  struct ipv4_pkt *ipv4_recv = (struct ipv4_pkt *)(recv_addr+sizeof(struct eth_pkt));
  struct tcp_pkt *tcp_recv = (struct tcp_pkt *)((uint)ipv4_recv + (ipv4_recv->ver&0xF)*4);

  struct eth_pkt *eth_send = (struct eth_pkt *)(send_addr);
  struct ipv4_pkt *ipv4_send = (struct ipv4_pkt *)(send_addr + sizeof(struct eth_pkt));
  struct tcp_pkt *tcp_send = (struct tcp_pkt *)((uint)ipv4_send + sizeof(struct ipv4_pkt));

  *send_size = sizeof(struct eth_pkt) + sizeof(struct ipv4_pkt) + sizeof(struct tcp_pkt) + payload_size;

  memmove(eth_send->dst_mac,eth_recv->src_mac,6);
  memmove(eth_send->src_mac,mac_addr,6);
  eth_send->type[0] = 0x08;
  eth_send->type[1] = 0x00;

  ipv4_send->ver = ((0x4)<<4)+((sizeof(struct ipv4_pkt)/4)&0xF);
  ipv4_send->srv_type = 0;
  ipv4_send->total_len = H2N_ushort(sizeof(struct ipv4_pkt) + sizeof(struct tcp_pkt) + payload_size);
  ipv4_send->id = send_id;
  send_id++;
  ipv4_send->fragment = H2N_ushort(0x0000);
  ipv4_send->ttl = 255;
  ipv4_send->protocol = IPV4_TYPE_TCP;
  memmove(ipv4_send->src_ip,my_ip,4);
  memmove(ipv4_send->dst_ip,ipv4_recv->src_ip,4);
  ipv4_send->chk_sum = 0;
  ipv4_send->chk_sum = H2N_ushort(ipv4_chksum((uint)ipv4_send));
  

  tcp_send->src_port = tcp_recv->dst_port;
  tcp_send->dst_port = tcp_recv->src_port;
  tcp_send->seq_num = H2N_uint(seq_num);
  tcp_send->ack_num = tcp_recv->seq_num + (1<<(8*3));

  tcp_send->code_bits[0] = 0;
  tcp_send->code_bits[1] = 0;
  tcp_send->code_bits[0] = 5<<4;
  tcp_send->code_bits[1] = pkt_type;

  tcp_send->window = H2N_ushort(14480);
  tcp_send->urgent_ptr = 0;
  tcp_send->chk_sum = 0;

  tcp_send->chk_sum = H2N_ushort(tcp_chksum((uint)(ipv4_send))+8);


}

ushort tcp_chksum(uint tcp_addr){
  struct ipv4_pkt *ipv4_p = (struct ipv4_pkt *)(tcp_addr);
  struct tcp_pkt *tcp_p = (struct tcp_pkt *)((uint)ipv4_p + sizeof(struct ipv4_pkt));
  struct tcp_dummy tcp_dummy;
  
  memmove(tcp_dummy.src_ip,my_ip,4);
  memmove(tcp_dummy.dst_ip,ipv4_p->src_ip,4);
  tcp_dummy.padding = 0;
  tcp_dummy.protocol = IPV4_TYPE_TCP;
  tcp_dummy.tcp_len = H2N_ushort(N2H_ushort(ipv4_p->total_len) - sizeof(struct ipv4_pkt));
  uint chk_sum = 0;
  uchar *bin = (uchar *)(&tcp_dummy);
  for(int i=0;i<6;i++){
    chk_sum += (bin[i*2]<<8)+bin[i*2+1];
  }

  bin = (uchar *)(tcp_p);
  for(int i=0;i<(N2H_ushort(tcp_dummy.tcp_len)/2);i++){
    chk_sum += (bin[i*2]<<8)+bin[i*2+1];
  }
  chk_sum += (chk_sum>>8*2);
  return ~(chk_sum);
}

void tcp_fin(){
  fin_flag =1;
}
