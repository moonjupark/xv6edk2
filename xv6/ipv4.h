#ifndef IPV4_H
#define IPV4_H
#include "types.h"

void ipv4_proc(uint buffer_addr);
ushort ipv4_chksum(uint ipv4_addr);
#define IPV4_TYPE_ICMP 0x1
#define IPV4_TYPE_TCP 0x6
#endif
