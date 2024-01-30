#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "x86.h"
#include "mmu.h"
#include "proc.h"
#include "mp_uefi.h"
#include "debug.h"

struct cpu cpus[NCPU];
int ncpu;
uchar ioapicid;
void mpinit_uefi(void)
{
  struct boot_param *boot_param = (struct boot_param *)P2V_WO(BOOTPARAM);
  struct uefi_madt *madt = (struct uefi_madt*)(P2V_WO(boot_param->madt_addr));

  uint i=sizeof(struct uefi_madt);
  struct uefi_lapic *lapic_entry;
  struct uefi_ioapic *ioapic;
  struct uefi_iso *iso;
  struct uefi_non_maskable_intr *non_mask_intr; 
  
  lapic = (uint *)(madt->lapic_addr);
  ncpu = 0;

  while(i<madt->len){
    uchar *entry_type = ((uchar *)madt)+i;
    switch(*entry_type){
      case 0:
        lapic_entry = (struct uefi_lapic *)entry_type;
        if(ncpu < NCPU) {
          cpus[ncpu].apicid = lapic_entry->lapic_id;
          ncpu++;
        }
        i += lapic_entry->record_len;
        break;

      case 1:
        ioapic = (struct uefi_ioapic *)entry_type;
        ioapicid = ioapic->ioapic_id;
        i += ioapic->record_len;
        break;

      case 2:
        iso = (struct uefi_iso *)entry_type;
        i += iso->record_len;
        break;

      case 4:
        non_mask_intr = (struct uefi_non_maskable_intr *)entry_type;
        i += non_mask_intr->record_len;
        break;

      case 5:
        i = i + 0xC;
        break;
    }
  }

}
