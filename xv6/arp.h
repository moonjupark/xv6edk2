#ifndef ARP_H
#define ARP_H

#include "pkts_hdr.h"
#include "types.h"
struct arp_entry {
  uchar ip[4];
  uchar mac[6];
  uchar use;
};

/*int arp_proc(unsigned char *recv,size_t recv_size,unsigned char **send,size_t *send_size,struct arp_entry *arp_table);
int arp_reply_pkt_create(struct arp_pkt *arp_recv,unsigned char **send,size_t *send_size);
int arp_broadcast(unsigned char **send,size_t *send_size,uint8_t ip);
int arp_table_update(struct arp_entry *arp_table,struct arp_pkt *recv_arp);
int arp_table_search(struct arp_entry *arp_table,uint8_t *ip);
int arp_table_initialize(struct arp_entry *arp_table);
int print_arp_table(struct arp_entry *arp_table);
*/
void arp_init();
void arp_broadcast();
void arp_scan();
int arp_proc(uint buffer_addr);
void print_arp_info(struct arp_pkt* arp_p);
void print_ipv4(uchar *ip);
void print_mac(uchar *mac);
void arp_table_update(struct arp_pkt *recv_arp);
int arp_table_search(uchar *ip);
void print_arp_table();
void arp_reply_pkt_create(struct arp_pkt *arp_recv,uint send,uint *send_size);


#define ARP_TABLE_MAX 64
#define ARP_HARDWARE_TYPE 0x0100
#define ARP_PROTOCOL_TYPE 0x0008
#define ARP_OPS_REQUEST 0x0100
#define ARP_OPS_REPLY 0x0200
#define ARP_UPDATED_TABLE 1
#define ARP_CREATED_REPLY 2
#endif
