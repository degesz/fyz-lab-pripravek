#include <Arduino.h>
#include <Wire.h>


#include "i2c_scanner.h"    
#include "usb_pd.h"
//#include "io_expander.h"
#include "charger.h"
#include "bms.h"
#include "cli.h"
#include "converter.h"
#include "measurement.h"

// Define I2C pins 
const int I2C_SDA_PIN = 1; 
const int I2C_SCL_PIN = 0;

TPS55288 converter;
bool stopLoop = 0;



void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for serial port to connect (optional)
  Serial.println(F("\nSystem Booting Up..."));

  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN); 
  
  Serial.println(F("Scanning I2C bus..."));
  scan(); // Call your I2C scanner function
  delay(100);


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



    //  bms_passthrough_FETs();

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
  bms_print();
  handle_cli();
  printMeasurements();
//    int batteryVoltage = readBQ28Z610Voltage();
//
//  if (batteryVoltage != -1) {
//    Serial.print("Napětí baterie: ");
//    Serial.print(batteryVoltage);
//    Serial.println(" mV");
//  } else {
//    Serial.println("Nepodařilo se přečíst napětí baterie.");
//  }

  


Serial.println();
     
  delay(700);
}