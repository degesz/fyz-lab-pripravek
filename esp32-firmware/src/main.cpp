#include <Arduino.h>
#include <Wire.h>

#include "neopixel.h"
#include "i2c_scanner.h"    
#include "usb_pd.h"
#include "charger.h"
#include "cli.h"
#include "converter.h"
#include "measurement.h"

// Define I2C pins 
//const int I2C_SDA_PIN = ; 
//const int I2C_SCL_PIN = ;

TPS55288 converter;
bool stopLoop = 0;



void setup() {
   Wire.begin(GPIO_NUM_34, GPIO_NUM_33); 

  Serial.begin(115200);
  while (!Serial); // Wait for serial port to connect (optional)
  Serial.println(F("\nSystem Booting Up..."));

 
  
delay(500);
  Serial.println(F("Scanning I2C bus..."));
  scan(); // Call your I2C scanner function
  delay(100);

  setup_neopixel();
  setup_usb();
  setup_cli();
  setup_charger();
  measurement_setup();


 // Enable the converter's output
  Serial.print("Enabling converter... ");
  if (converter.enable()) {
    Serial.println("Success!");
  } else {
    Serial.println("Failed!");
    while (1); // Halt on failure
  }

  converter.begin();

  uint8_t current_limit_val = 0x00; // Enable bit set + 30mV limit
  Serial.print("Setting current limit... ");
  if (converter.setCurrentLimit(current_limit_val)) {
    Serial.println("Success!");
  } else {
    Serial.println("Failed!");
  }

  converter.setVoltage(10);

    //  bq25703aRegisterDump();

}



void loop() {

while (stopLoop)
{  handle_cli();}

//  // Read and print status
//  uint8_t status = converter.getStatus();
//  Serial.print("Converter Status Register (0x07): 0x");
//  Serial.println(status, HEX);

  print_charger();
  
  handle_cli();
  printMeasurements();

  


Serial.println();
     
  delay(700);
}