#include <Arduino.h>
#include <Wire.h>



#include "i2c_scanner.h"    
#include "usb_pd.h"
#include "io_expander.h"
#include "charger.h"
#include "bms.h"
#include "cli.h"


// Define I2C pins 
const int I2C_SDA_PIN = 1; 
const int I2C_SCL_PIN = 0;




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

    //  bms_passthrough_FETs();

    //  bq25703aRegisterDump();

}



void loop() {


  print_charger();
  bms_print();
  handle_cli();
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
     
  delay(2000);
}