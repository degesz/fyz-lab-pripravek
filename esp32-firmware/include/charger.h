#ifndef CHARGER_H
#define CHARGER_H

#include <Arduino.h>
#include <Wire.h>
#include <Lorro_BQ25703A.h>


const uint8_t bq25703aRegisters[] = {
  0x00, // ChargeOption0
  0x02, // MaxChargeVoltage
  0x04, //Charge Current
  0x30, // ChargeOption1
  0x32, // ChargeOption2
  0x34, // ChargeOption3
  0x3A, // ADC Options
  0x20, // ChargerStatus
  0x22, // ProchStatus
  0x24, // IIN_DPM
  0x26, // ADCVBUS/PSYS
  0x28, // ADCIBAT
  0x2A, // ADCINCMPIN
  0x2C, // ADCVSYSVBAT
  0x06, // OTGVoltage
  0x08, // OTGCurrent
  0x0A, // InputVoltage
  0x0C, // MinSystemVoltage
  0x0E, // IIN_HOST
  0x2E // ManufacturerID (commented out as per original list)
  //0x2F  // DeviceID (commented out as per original list)
};

// Function to perform a register dump of the BQ25703A

void bq25703aRegisterDump();

void setup_charger();

void handle_charger();

void print_charger();

#endif // CHARGER_H