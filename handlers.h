#include "docs.h"
#include "log.h"
#include "lua.hpp"

const int GET = 0;
const int POST = 1;

#define METHOD(x) void x (Adafruit_CC3000_ClientRef &client, int method)

METHOD(give_200) {
  client.fastrprint("HTTP/1.0 200 OK\r\n"
                    "Server: MiniC HTTPD 0.1\r\n"
                    "\r\n");
}

METHOD(give_400) {
  client.fastrprint("HTTP/1.0 400 Bad Request\r\n"
                    "Server: MiniC HTTPD 0.1\r\n"
                    "\r\n");
}

METHOD(give_404) {
  client.fastrprint("HTTP/1.0 404 Not Found\r\n"
                      "Server: MiniC HTTPD 0.1\r\n"
                      "\r\n");
  client.fastrprint("Not found\r\n");
}

METHOD(give_200_gzip) {
    client.fastrprint("HTTP/1.0 200 OK\r\n"
                    "Server: MiniC HTTPD 0.1\r\n"
                    "Content-Encoding: gzip\r\n"
                    "\r\n");                     
}

// Helpers
int read_name(Adafruit_CC3000_ClientRef &client, char *str) {
     uint8_t i = 0,c = 0;
     for (i=0; i<16 && c != '='; i++) {
        c = client.read();
        str[i] = c;
      }
      
      str[i-1] = 0; // end the string  
      
      return i;
}

int read_value(Adafruit_CC3000_ClientRef &client, char *str) {
  uint8_t i = 0,c = 0;

  for (i=0; i<16 && c != '&' && client.available(); i++) {
    c = client.read();
    str[i] = c;
  }
      
  if (str[i-1] == '&')
    str[i-1] = 0;
    
  return i;
}

// METHODS
METHOD(root) {
  give_200_gzip(client, method);
  
  for (uint16_t i = 0; i < sizeof(doc_gzip); i++)
    client.write(pgm_read_byte(&doc_gzip[i]));
}

METHOD(interface) {
  give_200(client, method);
  
  for (uint16_t i = 0; i < sizeof(inter_head); i++)
    client.write(pgm_read_byte(&inter_head[i]));
  
  for (uint16_t i = 0; i < sizeof(l_prog_buff) && l_prog_buff[i]; i++)
    client.write(pgm_read_byte(&l_prog_buff[i]));  

  for (uint16_t i = 0; i < sizeof(inter_foot); i++)
    client.write(pgm_read_byte(&inter_foot[i]));
}



METHOD(output) {
  give_200(client, method);
  
  char *p = log_curpos+1;
  if (p - log_ringbuffer >= LOG_RINGSIZE)
    p = log_ringbuffer;
  
  
  client.fastrprint("<html><body><pre>");
  while(p != log_curpos) {
    if (*p)
      client.write(*p);

    p++;

    if (p - log_ringbuffer >= LOG_RINGSIZE)
      p = log_ringbuffer;    
  }
  client.fastrprint("</pre></body></html>");
}

// Setup the path resolver
struct method_resolve {
  const prog_char *path;
  void (*function)(Adafruit_CC3000_ClientRef&, int method);
};

#define FPATH(x) const prog_char x [] PROGMEM
#define ROUTE(m, p) {(const prog_char *) &p [0], m}

FPATH(root_path) = "/";
FPATH(output_path) = "/output";
FPATH(interface_path) = "/interface";

method_resolve GET_LIST[] = {
  ROUTE(root, root_path),
  ROUTE(output, output_path),
  ROUTE(interface, interface_path),
};

method_resolve POST_LIST[] = {
  ROUTE(root, root_path),
};

