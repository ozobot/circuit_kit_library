/// This example needs Freenove ESP32 WS2812 library to be installed

#include "drvkit.h"
#include "Freenove_WS2812_Lib_for_ESP32.h"

using namespace ozobot::drvkit;

#define LEDS_COUNT  1 /// We have only single LED on the board
#define LEDS_PIN	RGB_LED
#define CHANNEL		0

Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(LEDS_COUNT, LEDS_PIN, CHANNEL);

void setup() {
  strip.begin();

  pinMode(BUTTON, INPUT);
}

void loop() {
  /// Setup pixel 0 (we have only single LED) next color to low green (RGB)
  strip.setLedColorData(0, 0 , 30, 0);
  strip.show(); /// Write prepared colors to strip 
  delay(200);

  /// Setup pixel 0 (we have only single LED) next color to black (RGB)
  strip.setLedColorData(0, 0 , 0, 0);
  strip.show(); /// Write prepared colors to strip
  delay(200);
}
