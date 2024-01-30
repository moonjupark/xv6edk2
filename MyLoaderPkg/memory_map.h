#ifndef MEMORY_MAP_H_
#define MEMORY_MAP_H_

#include <Uefi/UefiBaseType.h>
#include <Protocol/SimpleFileSystem.h>

struct MemoryMap {
  UINTN BufferSize;
  VOID *Buffer;
  UINTN MapSize;
  UINTN MapKey;
  UINTN DescriptorSize;
  UINT32 DescriptorVersion;
};

EFI_STATUS GetMemoryMapFile();
#endif
