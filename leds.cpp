#include "leds.h"
#include <FastLED.h>

CRGB led_data[MAX_NUM_LEDS];

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
#define DATA_PIN 2
#define CLOCK_PIN 1

void setup_leds() {
  FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, RGB>(led_data, MAX_NUM_LEDS);  
};

void loop_leds() {
//  for(int i = 0; i < 10; i++)
//    led_data[i] = CRGB::White;
  
  FastLED.show();
};