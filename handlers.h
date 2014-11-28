#include "docs.h"

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
  client.fastrprint("HTTP/1.0 200 OK\r\n"
                    "Server: MiniC HTTPD 0.1\r\n"
                    "Content-Encoding: gzip\r\n"
                    "\r\n");
                      
                      
  for (uint16_t i = 0; i < sizeof(doc_gzip); i++)
    client.write(pgm_read_byte(&doc_gzip[i]));
  
  // This will eventually end up being a gziped set of docs
}

// Setup the path resolver
struct method_resolve {
  const prog_char *path;
  void (*function)(Adafruit_CC3000_ClientRef&, int method);
};

#define FPATH(x) const prog_char x [] PROGMEM
#define ROUTE(m, p) {(const prog_char *) &p [0], m}

FPATH(root_path) = "/";

method_resolve GET_LIST[] = {
  ROUTE(root, root_path),
};

method_resolve POST_LIST[] = {
  ROUTE(root, root_path),
};
