#ifndef CHARGER_H
#define CHARGER_H

#include <Arduino.h>
#include <Wire.h>

// BQ25703A I2C Address (7-bit)
extern const uint8_t BQ25703A_I2C_ADDR;

// Initializes I2C communication for the BQ25703A and checks connection
// Assumes Wire.begin() has been called in the main sketch if sda_pin and scl_pin are -1
// Otherwise, it will call Wire.begin(sda_pin, scl_pin)
// Returns true if device is detected, false otherwise.
bool bq25703a_init(int8_t sda_pin = -1, int8_t scl_pin = -1, uint32_t i2c_frequency = 400000L);

// Configures the BQ25703A for 2S Li-ion charging with specified parameters.
// Returns true on successful configuration, false otherwise.
bool bq25703a_configure_for_2s(float charge_voltage_V = 8.4f,
                               float charge_current_A = 1.0f,
                               float input_current_limit_A = 2.0f,
                               float min_system_voltage_V = 6.144f, // Typical for 2S
                               float vindpm_offset_V = 1.28f);     // VINDPM relative offset

// Prints various status information from the BQ25703A to Serial.
void bq25703a_print_status();

// Enables or disables charging.
// true to enable, false to disable.
void bq25703a_enable_charging(bool enable);

// Reads a 16-bit value from a BQ25703A register.
// Returns the register value, or 0xFFFF on error.
uint16_t bq25703a_read_register(uint8_t reg_addr);

// Writes a 16-bit value to a BQ25703A register.
void bq25703a_write_register(uint8_t reg_addr, uint16_t value);

// Checks if the BQ25703A device is connected and responding with correct IDs.
bool bq25703a_check_device();

#endif // CHARGER_H