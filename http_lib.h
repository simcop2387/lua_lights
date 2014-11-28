#ifndef __LUALIGHTS_HTTP_LIB_H
#define __LUALIGHTS_HTTP_LIB_H

#include <Adafruit_CC3000.h>

extern Adafruit_CC3000_Server httpServer;
void start_http();
void listen_http();

#endif