#include <Library/PrintLib.h>
#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/UefiBootServicesTableLib.h>
#include  <Protocol/BlockIo.h>
#include  <Protocol/LoadedImage.h>
#include  <Protocol/SimpleFileSystem.h>
#include  <Library/DevicePathLib.h>
#include  <Guid/FileInfo.h>
#include  <Library/MemoryAllocationLib.h>
#include  <Library/BaseMemoryLib.h>


#include "memory_map.h"

#define MEM_MAP_SIZE 8192

const CHAR16 *GetMemoryType(EFI_MEMORY_TYPE Type) {
    switch (Type) {
        case EfiReservedMemoryType: return L"EfiReservedMemoryType";
        case EfiLoaderCode: return L"EfiLoaderCode";
        case EfiLoaderData: return L"EfiLoaderData";
        case EfiBootServicesCode: return L"EfiBootServicesCode";
        case EfiBootServicesData: return L"EfiBootServicesData";
        case EfiRuntimeServicesCode: return L"EfiRuntimeServicesCode";
        case EfiRuntimeServicesData: return L"EfiRuntimeServicesData";
        case EfiConventionalMemory: return L"EfiConventionalMemory";
        case EfiUnusableMemory: return L"EfiUnusableMemory";
        case EfiACPIReclaimMemory: return L"EfiACPIReclaimMemory";
        case EfiACPIMemoryNVS: return L"EfiACPIMemoryNVS";
        case EfiMemoryMappedIO: return L"EfiMemoryMappedIO";
        case EfiMemoryMappedIOPortSpace: return L"EfiMemoryMappedIOPortSpace";
        case EfiPalCode: return L"EfiPalCode";
        case EfiPersistentMemory: return L"EfiPersistentMemory";
        case EfiMaxMemoryType: return L"EfiMaxMemoryType";
        default: return L"InvalidMemoryType";
    }
}


EFI_STATUS GetMemoryMapFile(){
  EFI_STATUS Status;
  struct MemoryMap MemoryMap = {0,NULL,0,0,0,0};
  
  MemoryMap.BufferSize = MEM_MAP_SIZE;
  MemoryMap.MapSize = MEM_MAP_SIZE;

  Status = gBS->AllocatePool(EfiLoaderData,MEM_MAP_SIZE,&MemoryMap.Buffer);
  if(EFI_ERROR(Status)){
    Print(L"Can not Allocate Memory Map Buffer\n");
    return Status;
  }

  Status = gBS->GetMemoryMap(
      &MemoryMap.MapSize,
      (EFI_MEMORY_DESCRIPTOR*)MemoryMap.Buffer,
      &MemoryMap.MapKey,
      &MemoryMap.DescriptorSize,
      &MemoryMap.DescriptorVersion);

  if(Status == EFI_BUFFER_TOO_SMALL){
    Print(L"Memory Map Buffer is too small\n");
    return Status;
  }
  
  EFI_FILE_PROTOCOL *Root;
  EFI_FILE_PROTOCOL *File;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFile;

  Status = gBS->LocateProtocol(&gEfiSimpleFileSystemProtocolGuid,NULL,(VOID **)&SimpleFile);
  
  if(EFI_ERROR(Status)) {
    Print(L"Failed to Locate EFI Simple File System Protocol\n");
    return Status;
  }
  Status = SimpleFile->OpenVolume(SimpleFile,&Root);

  if(EFI_ERROR(Status)) {
    Print(L"Failed to OpenVolume\n");
    return Status;
  }

  Status = Root->Open(Root, &File, L"\\memmap",
      EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE, 0);
  if(EFI_ERROR(Status)){
    Print(L"Failed to open memmap\n");
    return Status;
  }

  UINTN Len;
  CHAR8 Buffer[256];
  CHAR8 *Header =
        "| Index | Type     | PhysicalStart | VirtualStart  | NumberOfPages | Attribute |\n"
        "|-------|----------|---------------|---------------|---------------|-----------|\n";
  Len = AsciiStrLen(Header);
  Status = File->Write(File, &Len, Header);
  if (EFI_ERROR(Status)) {
    Print(L"Failed to write File\n");
    return Status;
  }

  EFI_PHYSICAL_ADDRESS Iter;
  UINTN i;
  for (Iter = (EFI_PHYSICAL_ADDRESS)MemoryMap.Buffer, i = 0;
    Iter < (EFI_PHYSICAL_ADDRESS)MemoryMap.Buffer + MemoryMap.MapSize;
    Iter += MemoryMap.DescriptorSize, i++) {
    EFI_MEMORY_DESCRIPTOR *Desc = (EFI_MEMORY_DESCRIPTOR*)Iter;
    Len = AsciiSPrint(Buffer, sizeof(Buffer),
          "| %2u | %x %-26ls | %08lx | %08lx | %4lx | %2ls %5lx |\n",
          i,
          Desc->Type, GetMemoryType(Desc->Type),
          Desc->PhysicalStart,
          Desc->VirtualStart,
          Desc->NumberOfPages,
          (Desc->Attribute & EFI_MEMORY_RUNTIME) ? L"RT" : L"",
          Desc->Attribute & 0xffffflu);
          Status = File->Write(File, &Len, Buffer);
          if (EFI_ERROR(Status)) {
            Print(L"Failed to write File\n");
            return Status;
          }
    }

  Status = File->Close(File);
  if(EFI_ERROR(Status)){
    Print(L"Failed to close File\n");
    return Status;
  }
  return Status;
}
