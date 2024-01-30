#ifndef HTTP_H
#define HTTP_H
#include "types.h"

void http_proc(uint recv, uint recv_size, uint send, uint *send_size);
int http_strcpy(char *dst,const char *src,int start_index);

#endif
