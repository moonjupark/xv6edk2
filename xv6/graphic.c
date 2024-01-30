#include "graphic.h"
#include "mp_uefi.h"
#include "memlayout.h"
#include "defs.h"
/*
 * i%4 = 0 : blue
 * i%4 = 1 : green
 * i%4 = 2 : red
 * i%4 = 3 : black
 */

struct gpu gpu;
void graphic_init(){
  struct boot_param *boot_param = (struct boot_param *)P2V_WO(BOOTPARAM);
  gpu.pvram_addr = boot_param->graphic_config.frame_base;
  gpu.vram_size = boot_param->graphic_config.frame_size;
  gpu.vvram_addr = DEVSPACE - gpu.vram_size;
  gpu.horizontal_resolution = (uint)(boot_param->graphic_config.horizontal_resolution & 0xFFFFFFFF);
  gpu.vertical_resolution = (uint)(boot_param->graphic_config.vertical_resolution & 0xFFFFFFFF);
  gpu.pixels_per_line = (uint)(boot_param->graphic_config.pixels_per_line & 0xFFFFFFFF);
}

void graphic_draw_pixel(int x,int y,struct graphic_pixel * buffer){
  int pixel_addr = (sizeof(struct graphic_pixel))*(y*gpu.pixels_per_line + x);
  struct graphic_pixel *pixel = (struct graphic_pixel *)(gpu.vvram_addr + pixel_addr);
  pixel->blue = buffer->blue;
  pixel->green = buffer->green;
  pixel->red = buffer->red;
}

void graphic_scroll_up(int height){
  int addr_diff = (sizeof(struct graphic_pixel))*gpu.pixels_per_line*height;
  memmove((unsigned int *)gpu.vvram_addr,(unsigned int *)(gpu.vvram_addr + addr_diff),gpu.vram_size - addr_diff);
  memset((unsigned int *)(gpu.vvram_addr + gpu.vram_size - addr_diff),0,addr_diff);
}

