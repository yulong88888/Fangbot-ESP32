#include <Adafruit_NeoPixel.h>
#include "config.h"

Adafruit_NeoPixel pixels(LED_RGB_NUM, PIN_LED_RGB, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 1000

void setup() { pixels.begin(); }

void loop() {
  pixels.clear();

  for (int i = 0; i < LED_RGB_NUM; i++) {
    // pixels.setPixelColor(i, pixels.Color(255, 0, 0));
    String temp = "0xFF8000";
    pixels.setPixelColor(i, temp.toFloat());

    pixels.show();

    // delay(DELAYVAL);
  }
}