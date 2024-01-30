#include "ipv4.h"
#include "defs.h"
#include "i8254.h"
#include "types.h"
#include "icmp.h"
#include "pkts_hdr.h"
#include "tcp.h"

extern uchar mac_addr[6];
extern uchar my_ip[4];

int ip_id = -1;
ushort send_id = 0;
void ipv4_proc(uint buffer_addr){
  struct ipv4_pkt *ipv4_p = (struct ipv4_pkt *)(buffer_addr+14);
  if(ip_id != ipv4_p->id && memcmp(my_ip,ipv4_p->src_ip,4) != 0){
    ip_id = ipv4_p->id;
      if(ipv4_p->protocol == IPV4_TYPE_ICMP){
        icmp_proc(buffer_addr);
      }else if(ipv4_p->protocol == IPV4_TYPE_TCP){
        tcp_proc(buffer_addr);
      }
  }
}

ushort ipv4_chksum(uint ipv4_addr){
  uchar* bin = (uchar *)ipv4_addr;
  uchar len = (bin[0]&0xF)*2;
  uint chk_sum = 0;
  for(int i=0;i<len;i++){
    chk_sum += (bin[i*2]<<8)+bin[i*2+1];
    if(chk_sum > 0xFFFF){
      chk_sum = (chk_sum&0xFFFF)+1;
    }
  }
  return ~(chk_sum);
}
