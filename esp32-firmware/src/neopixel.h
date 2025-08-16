
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define PIN GPIO_NUM_4         // Pin where NeoPixel is connected
#define NUMPIXELS 1   // Number of LEDs

Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup_neopixel() {
  strip.begin();
   uint8_t red = 100; // 10% of 255 (max brightness) ≈ 25
  uint8_t green = 0;
  uint8_t blue = 0;

  strip.setPixelColor(0, strip.Color(red, green, blue)); // Set first LED
  strip.show(); // Initialize all pixels to 'off'
}