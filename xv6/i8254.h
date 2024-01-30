#ifndef I8254_H
#define I8254_H
#include "pci.h"
#include "types.h"
void i8254_init(struct pci_dev *dev);
void i8254_init_recv();
void i8254_init_send();
uint i8254_read_eeprom(uint addr);
void i8254_recv();
int i8254_send(const uint pkt_addr,uint len);
void i8254_intr();

struct i8254_recv_desc {
  uint buf_addr;
  uint padding;
  ushort len;
  ushort chk_sum;
  uchar status;
  uchar errors;
  ushort special;
};
struct i8254_send_desc {
  uint buf_addr;
  uint padding;
  ushort len;
  uchar cso;
  uchar cmd;
  uchar sta;
  uchar css;
  ushort special;
};
#define I8254_DEVICE_ID 0x100E
#define I8254_VENDOR_ID 0x8086

#define I8254_RECV_DESC_NUM 256
#define I8254_SEND_DESC_NUM 256

#define I8254_CTRL_OFFSET 0x0
#define I8254_CTRL_FD (1<<0)
#define I8254_CTRL_LRST (1<<3)
#define I8254_CTRL_ASDE (1<<5)
#define I8254_CTRL_SLU (1<<6)
#define I8254_CTRL_ILOS (1<<7)
#define I8254_CTRL_FRCSPD (1<<11)
#define I8254_CTRL_FRCDPLX (1<<12)
#define I8254_CTRL_RST (1<<26)
#define I8254_CTRL_RFCE (1<<27)
#define I8254_CTRL_TFCE (1<<28)
#define I8254_CTRL_VME (1<<30)
#define I8254_CTRL_PHY_RST (1<<31)


#define I8254_RCTL_OFFSET 0x100
#define I8254_RCTL_EN (1<<1)
#define I8254_RCTL_SBP (1<<2)
#define I8254_RCTL_UPE (1<<3)
#define I8254_RCTL_MPE (1<<4)
#define I8254_RCTL_LPE (1<<5)
#define I8254_RCTL_BAM (1<<15)
#define I8254_RCTL_VFE (1<<18)
#define I8254_RCTL_CFIEN (1<<19)
#define I8254_RCTL_DPF (1<<22)
#define I8254_RCTL_PMCF (1<<23)
#define I8254_RCTL_BSEX (1<<25)
#define I8254_RCTL_SECRC (1<<26)
#define I8254_RCTL_BSIZE 0

#define I8254_TCTL_OFFSET 0x400
#define I8254_TCTL_EN (1<<1)
#define I8254_TCTL_PSP (1<<3)
#define I8254_TCTL_SWXOFF (1<<22)
#define I8254_TCTL_RTLC (1<<24)
#define I8254_TCTL_CT (0xF << 4)
#define I8254_TCTL_COLD (0x40 << 12)

#define I8254_RAH_AS_DEST (0<<16)
#define I8254_RAH_AS_SRC (1<<16)
#define I8254_RAH_AV (1<<31)


#define I8254_IMS_TXDW (1<<0)
#define I8254_IMS_TXQE (1<<1)
#define I8254_IMS_LSC (1<<2)
#define I8254_IMS_RXSEQ (1<<3)
#define I8254_IMS_RXDMT0 (1<<4)
#define I8254_IMS_RXO (1<<6)
#define I8254_IMS_RXT0 (1<<7)
#define I8254_IMS_MDAC (1<<9)
#define I8254_IMS_RXCFG (1<<10)
#define I8254_IMS_PHYINT (1<<12)
#define I8254_IMS_TXD_LOW (1<<15)
#define I8254_IMS_SRPD (1<<16)

#define I8254_TXDCTL_WTHRESH (1 << 16)
#define I8254_TXDCTL_GRAN_DESC (1 << 24)
#endif
