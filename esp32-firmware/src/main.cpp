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
#include "display.h"

TPS55288 converter;


bool stopLoop = 0;


void print_info()
{

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
//Ticker timer_display(update_display, 200, 0, MILLIS);

void setup()
{

  pinMode(GPIO_NUM_13, OUTPUT);   // turn on converter so it can be scanned
  digitalWrite(GPIO_NUM_13, HIGH);
  delay(10);

  Wire.begin(GPIO_NUM_34, GPIO_NUM_33, 100000);

  Serial.begin(115200);
  while (!Serial)
    ; // Wait for serial port to connect (optional)
  Serial.println(F("\nSystem Booting Up..."));

  delay(200);

  //  Serial.println(F("Scanning I2C bus..."));
  //  scan(); // scan the I2C bus
  delay(100);
  digitalWrite(GPIO_NUM_13, LOW);   // turn off converter

  
  
  setup_display();
  //show_splashscreen();
  setup_neopixel();
  setup_usb();
  setup_cli();
  setup_charger();
  measurement_setup();
  
  //converter.begin();
  
   

 timer_print.start();
 timer_LED.start();
 //timer_display.start();

 delay(100);
 ui_init();
}

void loop()
{

  while (stopLoop)
  {
    handle_cli();
  }
  
  handle_cli();
  timer_print.update();
  timer_LED.update();
 //timer_display.start();
  //update_display();
  lv_timer_handler(); /* let the GUI do its work */
  ui_tick();

}