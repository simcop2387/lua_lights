#ifndef __LUALIGHTS_LEDS_H
#define __LUALIGHTS_LEDS_H

#include <FastLED.h>

#define MAX_NUM_LEDS 500

extern CRGB led_data[];

void setup_leds();
void loop_leds();

#endif