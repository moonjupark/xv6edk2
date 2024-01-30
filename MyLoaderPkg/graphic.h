#ifndef GRAPHIC_H_
#define GRAPHIC_H_
#include <Uefi/UefiBaseType.h>

struct graphic_config {
  UINT64 frame_base;
  UINT64 frame_size;
  UINT64 horizontal_resolution;
  UINT64 vertical_resolution;
  UINT64 pixels_per_scan_line;
};

struct bmp_header {
  UINT8 sig[2];
  UINT32 file_size;
  UINT16 reserved1;
  UINT16 reserved2;
  UINT32 header_size;
  UINT32 info_header_size;
  UINT32 width;
  UINT32 height;
  UINT16 plane_num;
  UINT16 color_bit;
  UINT32 compression_type;
  UINT32 compression_size;
  UINT32 horizontal_pixel;
  UINT32 vertical_pixel;
  UINT32 color_num;
  UINT32 essentail_num;
};

struct graphic_pixel {
  UINT8 blue;
  UINT8 green;
  UINT8 red;
  UINT8 black;
};


EFI_STATUS GetGraphicMode(
    IN EFI_HANDLE ImageHandle,
    OUT struct graphic_config *graphic_config
    );
 EFI_STATUS DrawBMP(CHAR16 *file_name,struct graphic_config *graphic_config);
 void graphic_draw_pixel(UINTN x,UINTN y,UINTN bmp_addr,struct graphic_config *graphic_config);
#endif
