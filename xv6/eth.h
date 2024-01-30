#ifndef ETH_H
#define ETH_H
#include "types.h"

void eth_proc(uint buffer_addr);
ushort N2H_ushort(ushort value);
ushort H2N_ushort(ushort value);
uint H2N_uint(uint value);
uint N2H_uint(uint value);

#endif
