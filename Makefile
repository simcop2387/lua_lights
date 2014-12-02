BOARD_TAG    = teensy31
ARDUINO_LIBS = Adafruit_CC3000 FastLED SPI
ARDUINO_DIR = /home/ryan/arduino/1.0.6/arduino-1.0.6

CFLAGS += -Iinclude -Llib -llua -flto #-DSEND_NON_BLOCKING=1
CXXFLAGS += -Iinclude -Llib -llua -flto #-DSEND_NON_BLOCKING=1
# CPPFLAGS += -DSEND_NON_BLOCKING=1
# LDFLAGS += -flto
OTHER_OBJS += lib/liblua.a

include extern/Arduino-Makefile/Teensy.mk
