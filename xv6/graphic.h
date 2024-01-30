#ifndef GRAPHIC_H_
#define GRAPHIC_H_
#include "types.h"
#include "memlayout.h"

struct graphic_config {
  long long frame_base;
  long long frame_size;
  long long horizontal_resolution;
  long long vertical_resolution;
  long long pixels_per_line;
};

struct gpu {
  uint pvram_addr;
  uint vvram_addr;
  uint vram_size;
  uint horizontal_resolution;
  uint vertical_resolution;
  uint pixels_per_line;
};
struct graphic_pixel {
  uchar blue;
  uchar green;
  uchar red;
  uchar black;
};

void graphic_init();
void graphic_draw_box();
void graphic_draw_pixel(int x,int y,struct graphic_pixel *);
void graphic_scroll_up(int height);

extern struct gpu gpu;

#endif
