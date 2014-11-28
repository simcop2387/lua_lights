BOARD_TAG    = teensy31
ARDUINO_LIBS = Adafruit_CC3000 FastLED SPI
ARDUINO_DIR = /home/ryan/arduino/1.0.6/arduino-1.0.6

CFLAGS += -Iinclude -Llib -llua -flto
CXXFLAGS += -Iinclude -Llib -llua -flto
# LDFLAGS += -flto
OTHER_OBJS += lib/liblua.a

include extern/Arduino-Makefile/Teensy.mk
