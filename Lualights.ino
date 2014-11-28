/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  This example code is in the public domain.
 */

#include "lua.hpp"
#include "wifi.h"
#include "http_lib.h"
#include "log.h"
#include "leds.h"

void setup() {
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  //pinMode(13, OUTPUT);
  LogOut.begin(9600);
  delay(10000);
  
  l_init();
  setup_cc3000();
  start_http();
  setup_leds();
}

void loop() {
  l_frame();
  listen_http();
  loop_leds();
  
  //digitalWrite(13, HIGH);   // set the LED on
  //delay(100);              // wait for a second
  //digitalWrite(13, LOW);    // set the LED off
  //delay(100);              // wait for a second
}
