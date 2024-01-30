#ifndef MP_UEFI_H
#define MP_UEFI_H

#include "types.h"
#include "graphic.h"

struct uefi_madt {
  uchar sig[4];
  uint len;
  uchar rev;
  uchar chksum;
  uchar oem_id[6];
  uchar oem_table[8];
  uint oem_rev;
  uint creator_id;
  uint creator_rev;
  uint lapic_addr;
  uint flags;
};

struct __attribute__((packed)) uefi_lapic {
  uchar entry_type;
  uchar record_len;
  uchar processor_id;
  uchar lapic_id;
  uint flags;
};

struct __attribute__((packed)) uefi_ioapic {
  uchar entry_type;
  uchar record_len;
  uchar ioapic_id;
  uchar reserved;
  uint ioapic_addr;
  uint global_sys_intr_base;
};

struct __attribute__((packed)) uefi_iso {
  uchar entry_type;
  uchar record_len;
  uchar bus_src;
  uchar irq_src;
  uint gsi;
  uchar flags[2];
};

struct uefi_non_maskable_intr {
  uchar entry_type;
  uchar record_len;
  uchar acpi_processor_id;
  uchar flags[2];
  uchar lint;
};

struct __attribute__((packed)) gdt {
  uchar limit[2];
  uchar base1[2];
  uchar base2;
  uchar access_byte;
  uchar flags;
  uchar base3;
};

struct __attribute__((packed)) gdt_desc {
  uchar size[2];
  uchar gdt_addr[8];
};

struct __attribute__((packed)) boot_param {
  uint kernel_entry;
  uint padding1;
  uint madt_addr;
  uint padding2;
  struct graphic_config graphic_config;
  struct gdt bootstrap_gdt[3];
  struct gdt_desc bootstrap_gdt_desc;
  unsigned long long kernel_addr;

};

void mpinit_uefi(void);
#define UEFI_MP_LAPIC 0x0
#define UEFI_MP_IOAPIC 0x1

#endif
