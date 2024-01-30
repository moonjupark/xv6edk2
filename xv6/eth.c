#include "pkts_hdr.h"
#include "arp.h"
#include "types.h"
#include "eth.h"
#include "defs.h"
#include "ipv4.h"
void eth_proc(uint buffer_addr){
  struct eth_pkt *eth_pkt = (struct eth_pkt *)buffer_addr;
  uint pkt_addr = buffer_addr+sizeof(struct eth_pkt);
  if(eth_pkt->type[0] == 0x08 && eth_pkt->type[1] == 0x06){
    arp_proc(pkt_addr);
  }else if(eth_pkt->type[0] == 0x08 && eth_pkt->type[1] == 0x00){
    ipv4_proc(buffer_addr);
  }else{
  }
}

ushort N2H_ushort(ushort value){
  return (((value)&0xFF)<<8)+(value>>8);
}

ushort H2N_ushort(ushort value){
  return (((value)&0xFF)<<8)+(value>>8);
}

uint H2N_uint(uint value){
  return ((value&0xF)<<24)+((value&0xF0)<<8)+((value&0xF00)>>8)+((value&0xF000)>>24);
}

uint N2H_uint(uint value){
  return ((value&0xFF)<<24)+((value&0xFF00)<<8)+((value&0xFF0000)>>8)+((value&0xFF000000)>>24);
}
