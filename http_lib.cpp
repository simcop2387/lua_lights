#include "log.h"
#include "http_lib.h"
#include "handlers.h"
#include "dispatch.h"
#include <Adafruit_CC3000.h>

Adafruit_CC3000_Server httpServer(80);  
  
void start_http() {
  httpServer.begin();
}

void read_method(Adafruit_CC3000_ClientRef &client) {
  uint8_t method[9];
  
  for (uint8_t i = 0; i < 8; i++) {
    method[i] = client.read();
    if (method[i] == ' ') {
      method[i] = 0; 
      break;
    }
  }
  
//  LogOut.print(F("GOT METHOD: "));
//  LogOut.println((const char*) method);
  
  if (!strncmp((const char *) method, "GET", 8)) {
    handle_GET(client);
  } else if (!strncmp((const char *) method, "POST", 8)) {
    handle_POST(client);
  } else {
    // return a 400 to anything else.
    client.fastrprint("HTTP/1.0 400 Bad Request\r\n"
                      "Server: MiniC HTTPD 0.1\r\n"
                      "\r\n"
                      "The fuck is wrong with you trying other methods?\r\n");
  }
}

void listen_http() {
  // LogOut.println("Checking HTTP...");
  Adafruit_CC3000_ClientRef client = httpServer.available();
  // LogOut.println("Got Accept");
  
  if (client) {
    LogOut.println("GOT REQUEST, READING METHOD");
    read_method(client);
    client.close();
  }

  client.close();
  // LogOut.println("Done HTTP");
}
