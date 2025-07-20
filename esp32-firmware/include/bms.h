#pragma once

#include <Arduino.h>
#include <Wire.h>


// I2C adresa bq28z610 (výchozí je často 0x55)
#define BQ28Z610_ADDRESS 0x55

// Adresa registru pro napětí (v milivoltech)
// Podle technického referenčního manuálu bq28z610 (např. SLUUBW5)
// je příkaz pro čtení napětí 0x08/0x09 (Voltage())
#define BQ28Z610_VOLTAGE_CMD 0x08
#define BQ28Z610_CURRENT_CMD 0x0C
#define BQ28Z610_TEMP_INTERNAL_CMD 0x28
#define BQ28Z610_TEMP_CMD 0x06
#define BQ28Z610_CHARGING_VOLTAGE_CMD 0x30
#define BQ28Z610_CHARGING_CURRENT_CMD 0x32

#define CONTROL_CMD            0x00  //manufacturer access

#define CONTROL_SUBCMD_FETControl  0x0022  // Command to turn on FETs
#define CONTROL_SUBCMD_DischargeFET  0x0020  // Command to turn on FETs
#define CONTROL_SUBCMD_ChargeFET  0x001F  // Command to turn on FETs
#define CONTROL_SUBCMD_Gauging  0x0021  // Command to turn on FETs
#define CONTROL_SUBCMD_Reset  0x0041  // Command to turn on FETs


void bms_print();

int16_t readBMSRegister(uint8_t command);


void bms_passthrough_FETs();