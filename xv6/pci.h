#ifndef PCI_H_
#define PCI_H_
#include "types.h"
struct pci_dev {
  uchar bus_num;
  uchar device_num;
  uchar function_num;
  uint device_id;
  uint vendor_id;
  uchar base_class;
  uchar sub_class;
  uchar interface;
  uchar revision_id;
  uint bar0;
  uint bar1;
};

void pci_write_config(uint config);
uint pci_read_config();
void pci_access_config(uint bus_num,uint device_num,uint function_num,uint reg_addr,uint *data);
void pci_init();
void pci_init_device(uint bus_num,uint device_num,uint function_num);
void pci_test();
void pci_write_data(uint config);
void pci_write_config_register(uint bus_num,uint device_num,uint function_num,uint reg_addr,uint data);

#define PCI_CMD_BUS_MASTER (1<<2)
#define PCI_CMD_INTR_DISABLE (1<<10)
#endif
