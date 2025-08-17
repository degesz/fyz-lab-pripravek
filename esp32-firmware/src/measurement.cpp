#include "measurement.h"

Adafruit_INA3221 ina3221;


void measurement_setup(){
// Initialize the INA3221
  if (!ina3221.begin(0x40, &Wire)) { // can use other I2C addresses or buses
    Serial.println("Failed to find INA3221 chip, stopping program");
    while (1);
  }

  ina3221.setAveragingMode(INA3221_AVG_16_SAMPLES);

  ina3221.setShuntResistance(0, 0.01);
  ina3221.setShuntResistance(1, 0.05);
  ina3221.setShuntResistance(2, 1);

  // Set a power valid alert to tell us if ALL channels are between the two
  // limits:
  //ina3221.setPowerValidLimits(3.0 /* lower limit */, 15.0 /* upper limit */);
}



void printMeasurements(){
  for (uint8_t i = 0; i < 3; i++) {
    float voltage = ina3221.getBusVoltage(i);
    float current = ina3221.getCurrentAmps(i) * 1000; // Convert to mA

    Serial.print("CH ");
    Serial.print(i);
    Serial.print(":   Voltage = ");
    Serial.print(voltage, 2);
    Serial.print(" V, Current = ");
    Serial.print(current, 2);
    Serial.println(" mA");
  }

  Serial.println();
}