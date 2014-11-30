
#include <avr/pgmspace.h>

#define HANDLER(meth, num) void handle_ ## meth (Adafruit_CC3000_ClientRef &client) { \
  uint8_t buffer[64]; \
  memset(buffer, 0, 63); \
 \
  for (uint8_t p = 0; p < 63; p++) { \
    buffer[p] = client.read(); \
 \
    if (buffer[p] == ' ' || buffer[p] == '?') { \
      buffer[p] = 0; \
      break; \
    } \
  } \
  \
  debug_log("GOT URL: "); \
  debug_log((const char *) buffer); \
  \
  uint16_t length = sizeof(meth ## _LIST)/sizeof(meth ## _LIST[0]); \
  \
  debug_log("GET_LIST SIZE: "); \
  debug_log(length); \
  \
  for (size_t i = 0; i < length; i++) { \
    debug_log("Checking: "); \
    debug_log(i); \
    char path[64]; \
    strncpy_P(path, meth ## _LIST[i].path, 63); \
    debug_log("Path: "); \
    debug_log(path); \
    debug_logf((int) meth ## _LIST[i].path, HEX); \
    debug_logf((int) root_path, HEX); \
    \
    if (!strncmp_P((const char *)buffer, meth ## _LIST[i].path, 63)) { \
        skip_headers(client, num); \
        (* meth ## _LIST[i].function)(client, num); \
        client.println(F("\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n")); \
        return; \
      } \
  } \
  \
  give_404(client, 0); \
}



#define PUSH(x) for(int i=0; i<3; i++) buffer[i] = buffer[i+1]; buffer[3]=x;
#define CHECK_BUF (buffer[0] == '\r' && buffer[1] == '\n' && buffer[2] == '\r' && buffer[3] == '\n') 

METHOD(skip_headers) {
  uint8_t buffer[4] = {0,0,0,0};
  uint8_t c;
  
  // Look for a \r\n\r\n pattern in the incoming data, that'll be the end of the headers
  while(! CHECK_BUF) {
    c = client.read();
    PUSH(c);
  }
}

HANDLER(GET, 0);
HANDLER(POST, 1);
