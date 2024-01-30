#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/UefiBootServicesTableLib.h>
#include  <Protocol/BlockIo.h>
#include  <Protocol/LoadedImage.h>
#include  <Protocol/SimpleFileSystem.h>
#include  <Library/DevicePathLib.h>
#include  <Guid/FileInfo.h>
#include  <Guid/Acpi.h>
#include  <Library/MemoryAllocationLib.h>
#include  <Library/BaseMemoryLib.h>
#include "elf.h"
#include "relocate.h"
#include "memory_map.h"
#include "graphic.h"
#include "structs.h"

void set_gdt(struct gdt *gdt,UINT32 type,UINT32 base,UINT32 lim);

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS Status;
  CHAR16 *KernelPath = L"\\kernel";
  //CHAR16 *bmp_file = L"\\logo.bmp";
  EFI_PHYSICAL_ADDRESS BootParamAddr = 0x50000;
  EFI_PHYSICAL_ADDRESS KernelBaseAddr;

  RelocateELF(KernelPath, &KernelBaseAddr);
  GetMemoryMapFile();

  VOID *vendor_table;
  Status = EfiGetSystemConfigurationTable(&gEfiAcpiTableGuid,&vendor_table);
  struct rsdp *acpi_rsdp = (struct rsdp *)vendor_table;
  Print(L"Signature:");
  UINTN i;
  for(i=0;i<6;i++){
    Print(L"%c",acpi_rsdp->sig[i]);
  }
  Print(L"\n");
  Print(L"XSDT Address: %08x\n",acpi_rsdp->XsdtAddress);
  
  Status =  gBS->AllocatePages(AllocateAddress,EfiLoaderData,1,&BootParamAddr);
  if(EFI_ERROR(Status)){
    Print(L"Can not allocate boot param address\n");
  }
  struct xsdt *acpi_xsdt = (struct xsdt *)acpi_rsdp->XsdtAddress;
  struct madt *acpi_madt;
  struct boot_param *boot_param = (struct boot_param *)BootParamAddr;
  boot_param->kernel_entry = (UINT64)KernelBaseAddr;
  
  //GetGraphicMode(ImageHandle,&(boot_param->graphic_config));
  struct graphic_config gconfig;
  GetGraphicMode(ImageHandle,&(gconfig));
  boot_param->graphic_config = gconfig;
  Print(L"Frame Base Address: %x\n",boot_param->graphic_config.frame_base);
  Print(L"Frame Size: %x\n",boot_param->graphic_config.frame_size);
  Print(L"Horizontal Resolution: %x\n",boot_param->graphic_config.horizontal_resolution);
  Print(L"Vertical Resolution: %x\n",boot_param->graphic_config.vertical_resolution);
  Print(L"Pixels per Scan Line: %x\n",boot_param->graphic_config.pixels_per_scan_line);
  UINTN entries = (acpi_xsdt->h.len - sizeof(acpi_xsdt->h)) / 8;
  for(i=0;i<entries;i++){
    struct sdt_header *h = (struct sdt_header *)acpi_xsdt->pointer_others[i];
    if(!AsciiStrnCmp(h->sig,"APIC",4)){
      Print(L"Signature:");
      UINTN j;
      for(j=0;j<4;j++){
        AsciiPrint("%c",h->sig[j]);
      }
      Print(L"\n");
      Print(L"MADT Address: %p\n",h);
      acpi_madt = (struct madt *)h;
      boot_param->madt_addr = (UINT64)(boot_param+sizeof(struct boot_param));
      CopyMem((VOID *)(boot_param->madt_addr),(VOID *)h,h->len);
      Print(L"MADR_ADDR: %x \n",boot_param->madt_addr);
    }
  }
  Print(L"LAPIC Address: %04x\n",acpi_madt->lapic_addr);
  Print(L"LAPIC Flags: %04x\n",acpi_madt->flags);

  Print(L"MADT Length: %04x\n",acpi_madt->h.len);

  i=0x2C;
  while(i<acpi_madt->h.len){
    UINT8 *entry_type = ((UINT8 *)boot_param->madt_addr)+i;
    Print(L"EntryType:%x\n",*entry_type);
    switch(*entry_type){
      case 0:
        Print(L"Processor Local APIC  ");
        struct lapic *lapic = (struct lapic *)entry_type;
        Print(L"ACPI Processor ID: %x  ",lapic->processor_id);
        Print(L"APIC ID: %x  ",lapic->lapic_id);
        Print(L"Flag: %x\n",lapic->flags);
        i += lapic->entry.record_len;
        break;

      case 1:
        Print(L"I/O APIC  ");
        struct ioapic *ioapic = (struct ioapic *)entry_type;
        Print(L"I/O APIC ID: %x  ",ioapic->ioapic_id);
        Print(L"I/O APIC Address : %x\n",ioapic->ioapic_addr);
        i += ioapic->entry.record_len;
        break;

      case 2:
        Print(L"Interrupt Source Override ");
        struct iso *iso = (struct iso *)entry_type;
        Print(L"Bus Source: %x  ",iso->bus_src);
        Print(L"IRQ Source: %x\n",iso->irq_src);
        i += iso->entry.record_len;
        break;

      case 4:
        Print(L"Non-maskable interrupts  ");
        struct non_maskable_intr *non_mask_intr = (struct non_maskable_intr *)entry_type;
        Print(L"ACPI Processor ID: %x\n",non_mask_intr->acpi_processor_id);
        i += non_mask_intr->entry.record_len;
        break;

      case 5:
        Print(L"Local APIC Address Override\n");
        i = i + 0xC;
        break;
    }
  }


  boot_param->kernel_addr = KernelBaseAddr;
  Print(L"KernelAddress : %x\n",boot_param->kernel_addr);
  boot_param->bootstrap_gdt_desc.size = sizeof(struct gdt)*3;
  boot_param->bootstrap_gdt_desc.gdt_addr = (UINT64)boot_param->bootstrap_gdt;

  boot_param->bootstrap_gdt[0].limit = 0;
  boot_param->bootstrap_gdt[0].base1 = 0;
  boot_param->bootstrap_gdt[0].base2 = 0;
  boot_param->bootstrap_gdt[0].access_byte = 0;
  boot_param->bootstrap_gdt[0].flags = 0;
  boot_param->bootstrap_gdt[0].base3 = 0;

  set_gdt(&(boot_param->bootstrap_gdt[1]),STA_X|STA_R,0x0,0xffffffff);
  set_gdt(&(boot_param->bootstrap_gdt[2]),STA_W,0x0,0xffffffff);
  Print(L"BootStrap GDT Descriptor Address: %x\n",&(boot_param->bootstrap_gdt_desc));
  
  struct MemoryMap MemoryMap = {4096,NULL,4096,0,0,0};
  Status = gBS->AllocatePool(EfiLoaderData, MemoryMap.BufferSize, &MemoryMap.Buffer);
  if(EFI_ERROR(Status)){
    Print(L"Failed to allocate memory to get memory map\n");
    return Status;
  }
  Print(L"AllocatePool=%d\n",Status);
//  Status = DrawBMP(bmp_file,&(boot_param->graphic_config));
//  if(EFI_ERROR(Status)){
//    Print(L"Failed to draw bitmap file %s\n",bmp_file);
//  }
  gBS->GetMemoryMap(
            &MemoryMap.MapSize,
            (EFI_MEMORY_DESCRIPTOR*)MemoryMap.Buffer,
            &MemoryMap.MapKey,
            &MemoryMap.DescriptorSize,
            &MemoryMap.DescriptorVersion);
  
  Status = gBS->ExitBootServices(ImageHandle, MemoryMap.MapKey);
  if(EFI_ERROR(Status)){
    Print(L"Could not exit boot services : %r",Status);
  }
  asm("lgdt %0": : "m"(boot_param->bootstrap_gdt_desc));
  asm("push $0x08");
  asm("push %0": : "m"(boot_param->kernel_addr));
  asm("lretq");
  while(1);
}

void set_gdt(struct gdt *gdt,UINT32 type,UINT32 base,UINT32 lim){
  gdt->limit = (lim >> 12) & 0xFFFF;
  gdt->base1 = base & 0xFFFF;
  gdt->base2 = (base >> 16) & 0xFF;
  gdt->access_byte = 0x90 | type;
  gdt->flags = 0xC0 | ((lim >> 28) & 0xF);
  gdt->base3 = (base >> 24) & 0xFF;
}
