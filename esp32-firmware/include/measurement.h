#include <Arduino.h>
#include "Adafruit_INA3221.h"




void measurement_setup();
void printMeasurements();

// returns array of floats, in volts and miliamps
void getMeasurements(float* voltages, float* currents);