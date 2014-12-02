#include <Arduino.h>
#include "log.h"

char log_ringbuffer[LOG_RINGSIZE + 1];
char *log_curpos = log_ringbuffer;

void __debug_header(const char *file, const char *function, const int line) {
  LogOut.print(file);
  LogOut.print("<");
  LogOut.print(function);
  LogOut.print(">:");
  LogOut.print(line);
  LogOut.print(" ");
}

// debug_log only logs to the Serial port, output_log will output to the ring buffer available to the webserver
void _debug_log(const char *file, const char* function, const int line, const char *message) {
  __debug_header(file, function, line);
  LogOut.println(message);
}
void _debug_log(const char *file, const char* function, const int line, int message) {
  __debug_header(file, function, line);
  LogOut.println(message);
}
void _debug_log(const char *file, const char* function, const int line, unsigned int message) {
  __debug_header(file, function, line);
  LogOut.println(message);
}
void _debug_log(const char *file, const char* function, const int line, char message) {
  __debug_header(file, function, line);
  LogOut.println(message);
}
void _debug_log(const char *file, const char* function, const int line, int message, int format) {
  __debug_header(file, function, line);
  LogOut.println(message, format);
}

void _write_ring(const char *buff) {
  size_t len = strlen(buff);
  size_t p = log_curpos - log_ringbuffer;

  if (LOG_RINGSIZE - p > len + 1) { // We've got enough space to just write it directly
    memcpy(log_curpos, buff, len);
    log_curpos += len;
  } else { // Not enough space, we need to copy just enough and the go to the start of the buffer
    size_t left = LOG_RINGSIZE - p;
    memcpy(log_curpos, buff, left); // copy just the first part to the end of the buffer
    memcpy(log_ringbuffer, buff + left, len - left); // copy the rest to the start of the buffer
    log_curpos = log_ringbuffer + (len - left);
  }
  
  *log_curpos = 0; // set the current place to a nul, we gives us a good marker for the end
}

// These need to also output to the ring buffer, right now that isn't so important
void _output_log(const char *file, const char* function, const int line, const char *message) {  
  _debug_log(file, function, line, message); // Also put output to the serial connection
  _write_ring(message);
}
void _output_log(const char *file, const char* function, const int line, int message) {
  _debug_log(file, function, line, message); // Also put output to the serial connection
  
  char buff[16];
  sprintf(buff, "%d", message);
  _write_ring(buff);
}
void _output_log(const char *file, const char* function, const int line, unsigned int message) {
  _debug_log(file, function, line, message); // Also put output to the serial connection

  char buff[16];
  sprintf(buff, "%u", message);
  _write_ring(buff);
}
void _output_log(const char *file, const char* function, const int line, char message) {
  _debug_log(file, function, line, message); // Also put output to the serial connection

  char buff[2];
  buff[0] = message;
  buff[1] = 0;
  _write_ring(buff);
}
void _output_log(const char *file, const char* function, const int line, int message, int format) {
  _debug_log(file, function, line, message); // Also put output to the serial connection

  char buff[16];
  
  switch(format) {
    case BIN:
      sprintf(buff, "%X", message); // I don't care so much about binary, quick HACK
      break;
    case OCT:
      sprintf(buff, "%o", message);
      break;
    case DEC:
      sprintf(buff, "%d", message);
      break;
    case HEX:
      sprintf(buff, "%X", message);
      break;
  }
  

  _write_ring(buff);
}

