#include "pci.h"
#include "defs.h"
#include "types.h"
#include "i8254.h"

void pci_init(){
  uint data;
  for(int i=0;i<256;i++){
    for(int j=0;j<32;j++){
      for(int k=0;k<8;k++){
      pci_access_config(i,j,k,0,&data);
      if((data&0xFFFF) != 0xFFFF){
        pci_init_device(i,j,k);
      }
      }
    }
  }
}

void pci_write_config(uint config){
  asm("mov $0xCF8,%%edx\n\t"
      "mov %0,%%eax\n\t"
      "out %%eax,%%dx\n\t"
      : :"r"(config));
}

void pci_write_data(uint config){
  asm("mov $0xCFC,%%edx\n\t"
      "mov %0,%%eax\n\t"
      "out %%eax,%%dx\n\t"
      : :"r"(config));
}
uint pci_read_config(){
  uint data;
  asm("mov $0xCFC,%%edx\n\t"
      "in %%dx,%%eax\n\t"
      "mov %%eax,%0"
      :"=m"(data):);
  microdelay(200);
  return data;
}


void pci_test(){
  uint data = 0x80001804;
  pci_write_config(data);
}

void pci_access_config(uint bus_num,uint device_num,uint function_num,uint reg_addr,uint *data){
  uint config_addr = ((bus_num & 0xFF)<<16) | ((device_num & 0x1F)<<11) | ((function_num & 0x7)<<8) |
    (reg_addr & 0xFC) | 0x80000000;
  pci_write_config(config_addr);
  *data = pci_read_config();
}

void pci_write_config_register(uint bus_num,uint device_num,uint function_num,uint reg_addr,uint data){
  uint config_addr = ((bus_num & 0xFF)<<16) | ((device_num & 0x1F)<<11) | ((function_num & 0x7)<<8) |
    (reg_addr & 0xFC) | 0x80000000;
  pci_write_config(config_addr);
  pci_write_data(data);
}

struct pci_dev dev;
void pci_init_device(uint bus_num,uint device_num,uint function_num){
  uint data;
  dev.bus_num = bus_num;
  dev.device_num = device_num;
  dev.function_num = function_num;
  cprintf("PCI Device Found Bus:0x%x Device:0x%x Function:%x\n",bus_num,device_num,function_num);
  
  pci_access_config(bus_num,device_num,function_num,0,&data);
  uint device_id = data>>16;
  uint vendor_id = data&0xFFFF;
  dev.device_id = device_id;
  dev.vendor_id = vendor_id;
  cprintf("  Device ID:0x%x  Vendor ID:0x%x\n",device_id,vendor_id);
  
  pci_access_config(bus_num,device_num,function_num,0x8,&data);
  cprintf("  Base Class:0x%x  Sub Class:0x%x  Interface:0x%x  Revision ID:0x%x\n",
      data>>24,(data>>16)&0xFF,(data>>8)&0xFF,data&0xFF);
  dev.base_class = data>>24;
  dev.sub_class = (data>>16)&0xFF;
  dev.interface = (data>>8)&0xFF;
  dev.revision_id = data&0xFF;
  
  pci_access_config(bus_num,device_num,function_num,0x10,&data);
  dev.bar0 = data;
  pci_access_config(bus_num,device_num,function_num,0x14,&data);
  dev.bar1 = data;
  if(device_id == I8254_DEVICE_ID && vendor_id == I8254_VENDOR_ID){
    cprintf("E1000 Ethernet NIC Found\n");
    pci_access_config(bus_num,device_num,function_num,0xF0,&data);
    cprintf("Message Control:%x\n",data);
    i8254_init(&dev);
  }
}
