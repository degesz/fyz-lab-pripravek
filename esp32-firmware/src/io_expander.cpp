#include "io_expander.h"

Adafruit_MCP23X17 mcp;


void setup_io_expander(){
      if (!mcp.begin_I2C(0x26)) {
    Serial.println("----  Error setting up io expander.");
    while (1);
  }
  Serial.println("IO expander connected");
  Serial.println();


    mcp.pinMode(4, OUTPUT);
    mcp.pinMode(5, OUTPUT);
    mcp.pinMode(6, OUTPUT);
    mcp.pinMode(8, INPUT);


    mcp.digitalWrite(5, LOW);
    mcp.digitalWrite(6, LOW);
}

void handle_io_expander(){
 //   Serial.print("Charg_OK: ");
 //   Serial.println(mcp.digitalRead(8));
 mcp.digitalWrite(4, HIGH);
 delay(200);
  mcp.digitalWrite(4, LOW);
 delay(200);
}