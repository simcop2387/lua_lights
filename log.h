#ifndef __LUALIGHTS_LOG_H
#define __LUALIGHTS_LOG_H
// This line defines a "Uart" object to access the serial port
//  HardwareSerial CommOut = HardwareSerial();
// usb_serial_class LogOut = Serial;
#define LogOut Serial
#define LOG_RINGSIZE 8192

extern char log_ringbuffer[]; // ring buffer for the output log

#define debug_log(x) _debug_log(__BASE_FILE__, __FUNCTION__, __LINE__, x)
#define debug_logf(x, f) _debug_log(__BASE_FILE__, __FUNCTION__, __LINE__, x, f)
#define output_log(x) _output_log(__BASE_FILE__, __FUNCTION__, __LINE__, x)
#define output_logf(x, f) _output_log(__BASE_FILE__, __FUNCTION__, __LINE__, x, f)

// debug_log only logs to the Serial port, output_log will output to the ring buffer available to the webserver
void _debug_log(const char *file, const char* function, const int line, const char *message);
void _debug_log(const char *file, const char* function, const int line, int message);
void _debug_log(const char *file, const char* function, const int line, unsigned int message);
void _debug_log(const char *file, const char* function, const int line, char message);
void _debug_log(const char *file, const char* function, const int line, int message, int format);

// These need to also output to the ring buffer, right now that isn't so important
void _output_log(const char *file, const char* function, const int line, const char *message);
void _output_log(const char *file, const char* function, const int line, int message);
void _output_log(const char *file, const char* function, const int line, unsigned int message);
void _output_log(const char *file, const char* function, const int line, char message);
void _output_log(const char *file, const char* function, const int line, int message, int format);
#endif