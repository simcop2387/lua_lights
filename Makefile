BOARD_TAG    = teensy31
ARDUINO_LIBS =
ARDUINO_DIR = /home/ryan/arduino/1.0.6/arduino-1.0.6

CFLAGS += -Iinclude -Llib -llua
CXXFLAGS += -Iinclude -Llib -llua
LDFLAGS += -lc -lg
OTHER_OBJS += lib/liblua.a

include extern/Arduino-Makefile/Teensy.mk
