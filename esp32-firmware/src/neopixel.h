
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define PIN GPIO_NUM_9         // Pin where NeoPixel is connected
#define NUMPIXELS 1   // Number of LEDs

Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


// ==== Configurable settings ====
uint8_t baseRed   = 100;  // Base color (max intensity)
uint8_t baseGreen = 0;
uint8_t baseBlue  = 30;
uint8_t maxBrightness = 60;   // Max brightness (0–255)
uint16_t breathSpeed = 2000;   // half cycle in ms 




// ==== Internal variables ====
//unsigned long lastUpdate = 0;

void setup_neopixel() {
  strip.begin();
  strip.show(); // Initialize pixels off
}

void update_neopixel() {
  unsigned long now = millis();
  float phase = (now % breathSpeed) / (float)breathSpeed;  // 0.0 → 1.0

  // Breathing function: smooth sinusoidal curve
  float brightnessFactor = (exp(sin(phase * PI * 2)) - 0.36787944) * 0.425459064;  
  // Range ~0 → 1

  uint8_t brightness = (uint8_t)(brightnessFactor * maxBrightness);

  // Apply brightness scaling to base color
  uint8_t r = (baseRed   * brightness) / 255;
  uint8_t g = (baseGreen * brightness) / 255;
  uint8_t b = (baseBlue  * brightness) / 255;

  strip.setPixelColor(0, strip.Color(r, g, b));
  strip.show();
}