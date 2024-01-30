#include "graphic.h"
#include "font_bin.h"
#include "font.h"


struct graphic_pixel black_pixel = {0x0,0x0,0x0,0x0};
struct graphic_pixel white_pixel = {0xFF,0xFF,0xFF,0x0};
void font_render(int x,int y,int index){
  int bin;
  for(int i=0;i<30;i++){
    for(int j=14;j>-1;j--){
      bin = (font_bin[index-0x20][i])&(1 << j);
      if(bin == (1 << j)){
        graphic_draw_pixel(x+(14-j),y+i,&white_pixel);
      } else {
        graphic_draw_pixel(x+(14-j),y+i,&black_pixel);
      }
    }
  }
}

void font_render_string(char *string,int row){
  int i = 0;
  while(string[i] && i < 52){
    font_render(i*15+2,row*30,string[i]);
    i++;
  }
}
