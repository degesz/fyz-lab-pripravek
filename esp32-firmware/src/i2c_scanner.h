// SPDX-FileCopyrightText: 2023 Carter Nelson for Adafruit Industries
//
// SPDX-License-Identifier: MIT
// --------------------------------------
// i2c_scanner
//
// Modified from https://playground.arduino.cc/Main/I2cScanner/
// --------------------------------------
#include <Arduino.h>
#include <Wire.h>

// Set I2C bus to use: Wire, Wire1, etc.
#define WIRE Wire

void setup_scanner() {
  WIRE.begin(GPIO_NUM_1, GPIO_NUM_0);

  Serial.begin(115200);
  while (!Serial)
     delay(10);
  Serial.println("\nI2C Scanner");
}


void scan() {
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    WIRE.beginTransmission(address);
    error = WIRE.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.print("  !");

      if (address == 0x27) {
        Serial.print("  -> PCF IO Expander");
      } else if (address == 0x28) {
        Serial.print("  -> USB_PD");
      }else if (address == 0x26) {
        Serial.print("  -> MCP Expander");
      }else if (address == 0x75) {
        Serial.print("  -> Buck converter");
      }else if (address == 0x40) {
        Serial.print("  -> INA3221 ADC");
      }else if (address == 0x6B) {
        Serial.print("  -> Charger");
      }else if (address == 0x55) {
        Serial.print("  -> BMS");
      }

  Serial.println();
  nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

}
