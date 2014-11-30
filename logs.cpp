#include <Arduino.h>
#include "log.h"

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
  LogOut.println(message);
}

// These need to also output to the ring buffer, right now that isn't so important
void _output_log(const char *file, const char* function, const int line, const char *message) {
  _debug_log(file, function, line, message); // Also put output to the serial connection
}
void _output_log(const char *file, const char* function, const int line, int message) {
  _debug_log(file, function, line, message); // Also put output to the serial connection
}
void _output_log(const char *file, const char* function, const int line, unsigned int message) {
  _debug_log(file, function, line, message); // Also put output to the serial connection
}
void _output_log(const char *file, const char* function, const int line, char message) {
  _debug_log(file, function, line, message); // Also put output to the serial connection
}
void _output_log(const char *file, const char* function, const int line, int message, int format) {
  _debug_log(file, function, line, message); // Also put output to the serial connection
}

