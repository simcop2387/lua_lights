#ifndef __LUALIGHTS_LUA_H
#define __LUALIGHTS_LUA_H

#define MAX_PRGMSIZE 8192
extern char l_prog_buff[MAX_PRGMSIZE];

void l_init();
void l_frame();
void l_stop();
void l_start(const char *prgm);

#endif