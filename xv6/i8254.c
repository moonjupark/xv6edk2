#include "i8254.h"
#include "pci.h"
#include "defs.h"
#include "memlayout.h"
#include "defs.h"
#include "eth.h"
#include "arp.h"

uint base_addr;
uchar mac_addr[6] = {0};
uchar my_ip[4] = {10,0,1,10}; 
uint *intr_addr;
void i8254_init(struct pci_dev *dev){
  uint cmd_reg;
  //Enable Bus Master
  pci_access_config(dev->bus_num,dev->device_num,dev->function_num,0x04,&cmd_reg);
  cmd_reg = cmd_reg | PCI_CMD_BUS_MASTER;
  pci_write_config_register(dev->bus_num,dev->device_num,dev->function_num,0x04,cmd_reg);
  
  base_addr = PCI_P2V(dev->bar0);
  uint *ctrl = (uint *)base_addr;
  //Disable Interrupts
  uint *imc = (uint *)(base_addr+0xD8);
  *imc = 0xFFFFFFFF;
  
  //Reset NIC
  *ctrl = *ctrl | I8254_CTRL_RST;

  //Enable Interrupts
  *imc = 0xFFFFFFFF;

  //Enable Link
  *ctrl |= I8254_CTRL_SLU;
  
  //General Configuration
  *ctrl &= (~I8254_CTRL_PHY_RST | ~I8254_CTRL_VME | ~I8254_CTRL_ILOS);
  cprintf("E1000 General Configuration Done\n");
  intr_addr = (uint *)kalloc();
  *intr_addr = 0;
  cprintf("INTR_ADDR:%x\n",intr_addr);
  i8254_init_recv();
  i8254_init_send();
  cprintf("IP Address %d.%d.%d.%d\n",
      my_ip[0],
      my_ip[1],
      my_ip[2],
      my_ip[3]);
  *imc = 0x0;
}

void i8254_init_recv(){
  
  uint data_l = i8254_read_eeprom(0x0);
  mac_addr[0] = data_l&0xFF;
  mac_addr[1] = data_l>>8;
  uint data_m = i8254_read_eeprom(0x1);
  mac_addr[2] = data_m&0xFF;
  mac_addr[3] = data_m>>8;
  uint data_h = i8254_read_eeprom(0x2);
  mac_addr[4] = data_h&0xFF;
  mac_addr[5] = data_h>>8;
  
  cprintf("MAC Address %x:%x:%x:%x:%x:%x\n",
      mac_addr[0],
      mac_addr[1],
      mac_addr[2],
      mac_addr[3],
      mac_addr[4],
      mac_addr[5]);

  uint *ral = (uint *)(base_addr + 0x5400);
  uint *rah = (uint *)(base_addr + 0x5404);

  *ral = (data_l | (data_m << 16));
  *rah = (data_h | I8254_RAH_AS_DEST | I8254_RAH_AV);

  uint *mta = (uint *)(base_addr + 0x5200);
  for(int i=0;i<128;i++){
    mta[i] = 0;
  }

  uint *ims = (uint *)(base_addr + 0xD0);
  *ims = (I8254_IMS_RXT0 | I8254_IMS_RXDMT0 | I8254_IMS_RXSEQ | I8254_IMS_LSC | I8254_IMS_RXO);
  uint *ics = (uint *)(base_addr + 0xC8);
  *ics = (I8254_IMS_RXT0 | I8254_IMS_RXDMT0 | I8254_IMS_RXSEQ | I8254_IMS_LSC | I8254_IMS_RXO);



  uint *rxdctl = (uint *)(base_addr + 0x2828);
  *rxdctl = 0;

  uint *rctl = (uint *)(base_addr + 0x100);
  *rctl = (I8254_RCTL_UPE | I8254_RCTL_MPE | I8254_RCTL_BAM | I8254_RCTL_BSIZE | I8254_RCTL_SECRC);

  uint recv_desc_addr = (uint)kalloc();
  uint *rdbal = (uint *)(base_addr + 0x2800);
  uint *rdbah = (uint *)(base_addr + 0x2804);
  uint *rdlen = (uint *)(base_addr + 0x2808);
  uint *rdh = (uint *)(base_addr + 0x2810);
  uint *rdt = (uint *)(base_addr + 0x2818);

  *rdbal = V2P(recv_desc_addr);
  *rdbah = 0;
  *rdlen = sizeof(struct i8254_recv_desc)*I8254_RECV_DESC_NUM;
  *rdh = 0;
  *rdt = I8254_RECV_DESC_NUM;

  struct i8254_recv_desc *recv_desc = (struct i8254_recv_desc *)recv_desc_addr;
  for(int i=0;i<I8254_RECV_DESC_NUM;i++){
    recv_desc[i].padding = 0;
    recv_desc[i].len = 0;
    recv_desc[i].chk_sum = 0;
    recv_desc[i].status = 0;
    recv_desc[i].errors = 0;
    recv_desc[i].special = 0;
  }

  for(int i=0;i<(I8254_RECV_DESC_NUM)/2;i++){
    uint buf_addr = (uint)kalloc();
    if(buf_addr == 0){
      cprintf("failed to allocate buffer area\n");
      break;
    }
    recv_desc[i].buf_addr = V2P(buf_addr);
    recv_desc[i+1].buf_addr = V2P(buf_addr + 0x800);
  }

  *rctl |= I8254_RCTL_EN;
  cprintf("E1000 Recieve Initialize Done\n");
}

void i8254_init_send(){
  uint *txdctl = (uint *)(base_addr + 0x3828);
  *txdctl = (I8254_TXDCTL_WTHRESH | I8254_TXDCTL_GRAN_DESC);

  uint tx_desc_addr = (uint)kalloc();
  uint *tdbal = (uint *)(base_addr + 0x3800);
  uint *tdbah = (uint *)(base_addr + 0x3804);
  uint *tdlen = (uint *)(base_addr + 0x3808);

  *tdbal = V2P(tx_desc_addr);
  *tdbah = 0;
  *tdlen = sizeof(struct i8254_send_desc)*I8254_SEND_DESC_NUM;
  uint *tdh = (uint *)(base_addr + 0x3810);
  uint *tdt = (uint *)(base_addr + 0x3818);
  
  *tdh = 0;
  *tdt = 0;

  struct i8254_send_desc *send_desc = (struct i8254_send_desc *)tx_desc_addr;
  for(int i=0;i<I8254_SEND_DESC_NUM;i++){
    send_desc[i].padding = 0;
    send_desc[i].len = 0;
    send_desc[i].cso = 0;
    send_desc[i].cmd = 0;
    send_desc[i].sta = 0;
    send_desc[i].css = 0;
    send_desc[i].special = 0;
  }

  for(int i=0;i<(I8254_SEND_DESC_NUM)/2;i++){
    uint buf_addr = (uint)kalloc();
    if(buf_addr == 0){
      cprintf("failed to allocate buffer area\n");
      break;
    }
    send_desc[i].buf_addr = V2P(buf_addr);
    send_desc[i+1].buf_addr = V2P(buf_addr + 0x800);
  }

  uint *tctl = (uint *)(base_addr + 0x400);
  *tctl = (I8254_TCTL_EN | I8254_TCTL_PSP | I8254_TCTL_COLD | I8254_TCTL_CT);

  uint *tipg = (uint *)(base_addr + 0x410);
  *tipg = (10 | (10<<10) | (10<<20));
  cprintf("E1000 Transmit Initialize Done\n");

}
uint i8254_read_eeprom(uint addr){
  uint *eerd = (uint *)(base_addr + 0x14);
  *eerd = (((addr & 0xFF) << 8) | 1);
  while(1){
    cprintf("");
    volatile uint data = *eerd;
    if((data & (1<<4)) != 0){
      break;
    }
  }

  return (*eerd >> 16) & 0xFFFF;
}
void i8254_recv(){
  uint *rdh = (uint *)(base_addr + 0x2810);
  uint *rdt = (uint *)(base_addr + 0x2818);
//  uint *torl = (uint *)(base_addr + 0x40C0);
//  uint *tpr = (uint *)(base_addr + 0x40D0);
//  uint *icr = (uint *)(base_addr + 0xC0);
  uint *rdbal = (uint *)(base_addr + 0x2800);
  struct i8254_recv_desc *recv_desc = (struct i8254_recv_desc *)(P2V(*rdbal));
  while(1){
    int rx_available = (I8254_RECV_DESC_NUM - *rdt + *rdh)%I8254_RECV_DESC_NUM;
    if(rx_available > 0){
      uint buffer_addr = P2V_WO(recv_desc[*rdt].buf_addr);
      *rdt = (*rdt + 1)%I8254_RECV_DESC_NUM;
      eth_proc(buffer_addr);
    }
    if(*rdt == *rdh) {
      (*rdt)--;
    }
  }
}

int i8254_send(const uint pkt_addr,uint len){
  uint *tdh = (uint *)(base_addr + 0x3810);
  uint *tdt = (uint *)(base_addr + 0x3818);
  uint *tdbal = (uint *)(base_addr + 0x3800);
  struct i8254_send_desc *txdesc = (struct i8254_send_desc *)P2V_WO(*tdbal);
  int tx_available = I8254_SEND_DESC_NUM - ((I8254_SEND_DESC_NUM - *tdh + *tdt) % I8254_SEND_DESC_NUM);
  uint index = *tdt%I8254_SEND_DESC_NUM;
  if(tx_available > 0) {
    memmove(P2V_WO((void *)txdesc[index].buf_addr),(void *)pkt_addr,len);
    txdesc[index].len = len;
    txdesc[index].sta = 0;
    txdesc[index].css = 0;
    txdesc[index].cmd = 0xb;
    txdesc[index].special = 0;
    txdesc[index].cso = 0;
    *tdt = (*tdt + 1)%I8254_SEND_DESC_NUM;
    return len;
  }else{
    return -1;
  }
  return 0;
}

void i8254_intr(){
  *intr_addr = 0xEEEEEE;
}
