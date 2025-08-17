#include <Arduino.h>
#include <Wire.h>
#include "Ticker.h"

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



void print_info(){
  
  print_charger();
    Serial.println("----------------");
  printMeasurements();
    Serial.println("----------------");
  converter.print_converter();
    Serial.println("----------------");
  Serial.println();
     
}



Ticker timer_print(print_info, 700, 0, MILLIS);
Ticker timer_LED(update_neopixel, 30, 0, MILLIS);

void setup() {
   pinMode(GPIO_NUM_13, OUTPUT);    //Enable buck-boost
   //digitalWrite(GPIO_NUM_13, HIGH); //

   Wire.begin(GPIO_NUM_34, GPIO_NUM_33); 

  Serial.begin(115200);
  while (!Serial); // Wait for serial port to connect (optional)
  Serial.println(F("\nSystem Booting Up..."));
  
  delay(200);

  Serial.println(F("Scanning I2C bus..."));
  scan(); // Call your I2C scanner function
  delay(100);

  setup_neopixel();
  setup_usb();
  setup_cli();
  setup_charger();
  measurement_setup();


  converter.begin();

 // Enable the converter's output
 //Serial.print("Enabling converter... ");
 //converter.enable()


  Serial.print("Setting current limit 650mA... ");
  if (converter.setCurrentLimit(650)) {
    Serial.println("Success!");
  } else {
    Serial.println("Failed!");
  }

  converter.setVoltage(5);

    //  bq25703aRegisterDump();


  timer_print.start();  
  timer_LED.start();

}



void loop() {

while (stopLoop)
{  handle_cli();}


  handle_cli();
  timer_print.update();  
  timer_LED.update();


//  update_neopixel();  // Call every loop



 // delay(700);
}