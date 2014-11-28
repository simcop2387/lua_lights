const int GET = 0;
const int POST = 1;

#define METHOD(x) void x (Adafruit_CC3000_ClientRef &client, int method)

METHOD(give_200) {
  client.fastrprint(F("HTTP/1.0 200 OK\r\n"
                      "Server: MiniC HTTPD 0.1\r\n"
                      "\r\n"));
}

METHOD(give_400) {
  client.fastrprint(F("HTTP/1.0 400 Bad Request\r\n"
                      "Server: MiniC HTTPD 0.1\r\n"
                      "\r\n"));
}

METHOD(give_404) {
  client.fastrprint(F("HTTP/1.0 404 Not Found\r\n"
                      "Server: MiniC HTTPD 0.1\r\n"
                      "\r\n"));
  client.fastrprint(F("Not found\r\n"));
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
  client.fastrprint(F("HTTP/1.0 200 OK\r\n"
                      "Server: MiniC HTTPD 0.1\r\n"
                      "Content-Encoding: gzip\r\n"
                      "\r\n"));  
                      
                      
  for (uint16_t i = 0; i < sizeof(doc_gzip); i++)
    client.write(pgm_read_byte(&doc_gzip[i]));
  
  // This will eventually end up being a gziped set of docs
}

METHOD(setting_color_current);
METHOD(setting_color_current) {
  if (method == GET) {
    give_200(client, method);
    
    client.fastrprint(F("{\"r\": "));
    // TODO fix this for 24bit color!
    client.print((settings.current_color >> 5) & 0x1F, DEC);
    client.fastrprint(F(", \"g\": "));
    client.print((settings.current_color) & 0x1F, DEC);    
    client.fastrprint(F(", \"b\": "));
    client.print((settings.current_color >> 10) & 0x1F, DEC);
    client.fastrprint(F("}"));
  } else if (method == POST) {
    uint8_t r, g, b, a; // a is which are initialized
    r = g = b = a = 0;
    
    while(client.available()) {
      uint8_t type[16] = {0};
      uint8_t q = 0;
      
      read_name(client, (char *) type);
      
      if (!strncmp("r", (char *) type, 16)) {
        q = 1;
      } else if (!strncmp("g", (char *) type, 16)) {
        q = 2;
      } else if (!strncmp("b", (char *) type, 16)) {
        q = 3;
      }
      
      read_value(client, (char *) type);
      
      switch(q) {
        case 1:
          r = (uint8_t) atoi((char *) type);
          a |= 1;
          break;
        case 2:
          g = (uint8_t) atoi((char *) type);
          a |= 2;
          break;
        case 3:
          b = (uint8_t) atoi((char *) type);
          a |= 4;
          break;
           
        default:
          break;
      }
    }
    
    // We got all three variables!
    if (a == 7) {
      settings.current_color = Color(r,g,b);
    }
    
    setting_color_current(client, GET);
  }
}

METHOD(setting_dark_current);
METHOD(setting_dark_current) {
  if (method == GET) {
    give_200(client, method);
    
    client.fastrprint(F("{\"dark\": "));
    client.print(settings.dark_level, DEC);
    client.fastrprint(F("}"));
  } else if (method == POST) {
    uint8_t a; // a is which are initialized
    uint16_t dark;
    dark = a = 0;
    
    while(client.available()) {
      uint8_t type[16] = {0};
      uint8_t q;
      
      read_name(client, (char *) type);
      
      if (!strncmp("dark", (char *) type, 16)) {
        q = 1;
      }
      
      read_value(client, (char *) type);

      switch(q) {
        case 1:
          dark = (uint8_t) atoi((char *) type);
          a |= 1;
          break;
           
        default:
          break;
      }
    }
    
    // We got all three variables!
    if (a == 1) {
      settings.dark_level = dark;
    }
    
    setting_dark_current(client, GET);
  }
}

METHOD(setting_time_current);
METHOD(setting_time_current) {
  if (method == GET) {
    give_200(client, method);
    
    client.fastrprint(F("{\"time\": "));
    client.print(settings.time_out, DEC);
    client.fastrprint(F("}"));
  } else if (method == POST) {
    uint8_t a; // a is which are initialized
    uint16_t time;
    time = a = 0;
    
    while(client.available()) {
      uint8_t type[16] = {0};
      uint8_t q;
      
      read_name(client, (char *) type);
      
      if (!strncmp("time", (char *) type, 16)) {
        q = 1;
      }
      
      read_value(client, (char *) type);

      switch(q) {
        case 1:
          time = (uint8_t) atoi((char *) type);
          a |= 1;
          break;
           
        default:
          break;
      }
    }
    
    // We got all three variables!
    if (a == 1) {
      settings.time_out = time;
    }
    
    setting_time_current(client, GET);
  }
}

METHOD(status_dark) {
  if (method == GET) {
    give_200(client, method);
    
    client.fastrprint(F("{\"dark\": "));
    client.print(get_light(), DEC);
    client.fastrprint(F("}"));
  } else {
    give_400(client, method);
  }
}

METHOD(status_time) {
  if (method == GET) {
    give_200(client, method);
    
    client.fastrprint(F("{\"time\": "));
    client.print(motion_countup, DEC);
    client.fastrprint(F("}"));
  } else {
    give_400(client, method);
  }
}

METHOD(status_state) {
  if (method == GET) {
    give_200(client, method);
    
    client.fastrprint(F("{\"state\": "));
    client.print(state, DEC);
    client.fastrprint(F("}"));
  } else {
    uint8_t a; // a is which are initialized
    uint16_t state_v;
    state_v = a = 0;
    
    while(client.available()) {
      uint8_t type[16] = {0};
      uint8_t q;
      
      read_name(client, (char *) type);
      
      if (!strncmp("state", (char *) type, 16)) {
        q = 1;
      }
      
      read_value(client, (char *) type);

      switch(q) {
        case 1:
          state_v = (uint8_t) atoi((char *) type);
          a |= 1;
          break;
           
        default:
          break;
      }
    }
    
    // We got all three variables!
    if (a == 1) {
      state = (e_state) state_v;
    }
    
    status_state(client, GET);    
  }
}

// Setup the path resolver
struct method_resolve {
  const prog_char *path;
  void (*function)(Adafruit_CC3000_ClientRef&, int method);
};

#define FPATH(x) const prog_char x [] PROGMEM
#define ROUTE(m, p) {(const prog_char *) &p [0], m}

FPATH(root_path) = "/";
FPATH(setting_color_path) = "/settings/current/color";
FPATH(setting_dark_path)  = "/settings/current/dark";
FPATH(setting_time_path)  = "/settings/current/time";
FPATH(status_dark_path)  = "/status/dark";
FPATH(status_time_path)  = "/status/time";
FPATH(status_state_path)  = "/status/state";

method_resolve GET_LIST[] = {
  ROUTE(root, root_path),
  ROUTE(setting_color_current, setting_color_path),
  ROUTE(setting_dark_current, setting_dark_path),
  ROUTE(setting_time_current, setting_time_path),
  ROUTE(status_dark, status_dark_path),
  ROUTE(status_time, status_time_path),
  ROUTE(status_state, status_state_path),
};

method_resolve POST_LIST[] = {
  ROUTE(root, root_path),
  ROUTE(setting_color_current, setting_color_path),
  ROUTE(setting_dark_current, setting_dark_path),
  ROUTE(setting_time_current, setting_time_path),
  ROUTE(status_state, status_state_path),
};

