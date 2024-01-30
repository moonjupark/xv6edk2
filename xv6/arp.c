#include "pkts_hdr.h"
#include "defs.h"
#include "arp.h"
#include "i8254.h"
extern uchar mac_addr[6];
extern uchar my_ip[4];

struct arp_entry arp_table[ARP_TABLE_MAX] = {0};

int arp_proc(uint buffer_addr){
  struct arp_pkt *arp_p = (struct arp_pkt *)(buffer_addr);
  if(arp_p->hrd_type != ARP_HARDWARE_TYPE) return -1;
  if(arp_p->pro_type != ARP_PROTOCOL_TYPE) return -1;
  if(arp_p->hrd_len != 6) return -1;
  if(arp_p->pro_len != 4) return -1;
  if(memcmp(my_ip,arp_p->dst_ip,4) != 0 && memcmp(my_ip,arp_p->src_ip,4) != 0) return -1;
  if(arp_p->op == ARP_OPS_REQUEST && memcmp(my_ip,arp_p->dst_ip,4) == 0){
    uint send = (uint)kalloc();
    uint send_size=0;
    arp_reply_pkt_create(arp_p,send,&send_size);
    i8254_send(send,send_size);
    kfree((char *)send);
    return ARP_CREATED_REPLY;
  }else if(arp_p->op == ARP_OPS_REPLY && memcmp(my_ip,arp_p->dst_ip,4) == 0){
    cprintf("ARP TABLE UPDATED\n");
    arp_table_update(arp_p);
    return ARP_UPDATED_TABLE;
  }else{
    return -1;
  }
}

void arp_scan(){
  uint send_size;
  for(int i=0;i<256;i++){
    uint send = (uint)kalloc();
    arp_broadcast(send,&send_size,i);
    uint res = i8254_send(send,send_size);
    while(res == -1){
      microdelay(1);
      res = i8254_send(send,send_size);
    }
    kfree((char *)send);
  }
}

void arp_broadcast(uint send,uint *send_size,uint ip){
  uchar dst_ip[4] = {10,0,1,ip};
  uchar dst_mac_eth[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
  uchar dst_mac_arp[6] = {0,0,0,0,0,0};
  
  *send_size = sizeof(struct eth_pkt) + sizeof(struct arp_pkt);

  struct eth_pkt *reply_eth = (struct eth_pkt *)send;
  struct arp_pkt *reply_arp = (struct arp_pkt *)(send + sizeof(struct eth_pkt));
  
  reply_eth->type[0] = 0x08;
  reply_eth->type[1] = 0x06;
  memmove(reply_eth->dst_mac,dst_mac_eth,6);
  memmove(reply_eth->src_mac,mac_addr,6);

  reply_arp->hrd_type = ARP_HARDWARE_TYPE;
  reply_arp->pro_type = ARP_PROTOCOL_TYPE;
  reply_arp->hrd_len = 6;
  reply_arp->pro_len = 4;
  reply_arp->op = ARP_OPS_REQUEST;
  memmove(reply_arp->dst_mac,dst_mac_arp,6);
  memmove(reply_arp->dst_ip,dst_ip,4);
  memmove(reply_arp->src_mac,mac_addr,6);
  memmove(reply_arp->src_ip,my_ip,4);
}

void arp_table_update(struct arp_pkt *recv_arp){
  int index = arp_table_search(recv_arp->src_ip);
  if(index > -1){
    memmove(arp_table[index].mac,recv_arp->src_mac,6);
  }else{
    index += 1;
    index = -index;
    memmove(arp_table[index].mac,recv_arp->src_mac,6);
    memmove(arp_table[index].ip,recv_arp->src_ip,4);
    arp_table[index].use = 1;
  }
  print_arp_table(arp_table);
}

int arp_table_search(uchar *ip){
  int empty=1;
  for(int i=0;i<ARP_TABLE_MAX;i++){
    if(memcmp(arp_table[i].ip,ip,4) == 0){
      return i;
    }
    if(arp_table[i].use == 0 && empty == 1){
      empty = -i;
    }
  }
  return empty-1;
}

void print_arp_table(){
  for(int i=0;i < ARP_TABLE_MAX;i++){
    if(arp_table[i].use != 0){
      cprintf("Entry Num: %d ",i);
      print_ipv4(arp_table[i].ip);
      cprintf(" ");
      print_mac(arp_table[i].mac);
      cprintf("\n");
    }
  }
}


void arp_reply_pkt_create(struct arp_pkt *arp_recv,uint send,uint *send_size){
  *send_size = sizeof(struct eth_pkt) + sizeof(struct arp_pkt);
  
  struct eth_pkt *reply_eth = (struct eth_pkt *)send;
  struct arp_pkt *reply_arp = (struct arp_pkt *)(send + sizeof(struct eth_pkt));

  reply_eth->type[0] = 0x08;
  reply_eth->type[1] = 0x06;
  memmove(reply_eth->dst_mac,arp_recv->src_mac,6);
  memmove(reply_eth->src_mac,mac_addr,6);

  reply_arp->hrd_type = ARP_HARDWARE_TYPE;
  reply_arp->pro_type = ARP_PROTOCOL_TYPE;
  reply_arp->hrd_len = 6;
  reply_arp->pro_len = 4;
  reply_arp->op = ARP_OPS_REPLY;
  memmove(reply_arp->dst_mac,arp_recv->src_mac,6);
  memmove(reply_arp->dst_ip,arp_recv->src_ip,4);
  memmove(reply_arp->src_mac,mac_addr,6);
  memmove(reply_arp->src_ip,my_ip,4);
}

void print_arp_info(struct arp_pkt* arp_p){
  cprintf("--------Source-------\n");
  print_ipv4(arp_p->src_ip);
  cprintf("\n");
  print_mac(arp_p->src_mac);
  cprintf("\n");
  cprintf("-----Destination-----\n");
  print_ipv4(arp_p->dst_ip);
  cprintf("\n");
  print_mac(arp_p->dst_mac);
  cprintf("\n");
  cprintf("Operation: ");
  if(arp_p->op == ARP_OPS_REQUEST) cprintf("Request\n");
  else if(arp_p->op == ARP_OPS_REPLY) {
    cprintf("Reply\n");
  }
  cprintf("\n");
}

void print_ipv4(uchar *ip){
  cprintf("IP address: %d.%d.%d.%d",ip[0],ip[1],ip[2],ip[3]);
}

void print_mac(uchar *mac){
  cprintf("MAC address: %x:%x:%x:%x:%x:%x",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
}

