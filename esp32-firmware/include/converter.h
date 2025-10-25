#ifndef CONVERTER_H
#define CONVERTER_H

#include <Arduino.h>
#include <Wire.h>
#include <math.h>
#include <ui.h>

// The default 7-bit I2C address for the TPS55288
#define TPS55288_I2C_ADDRESS 0x75

// TPS55288 Register Map (as per datasheet slvsf01b)
#define TPS55288_REF_LSB      0x00 // VREF LSB
#define TPS55288_REF_MSB      0x01 // VREF MSB
#define TPS55288_IOUT_LIMIT   0x02 // Output Current Limit Setting
#define TPS55288_VOUT_SR      0x03 // Slew Rate Control
#define TPS55288_VOUT_FS      0x04 // Feedback Selection
#define TPS55288_CDC          0x05 // Cable Droop Compensation
#define TPS55288_MODE         0x06 // Mode Control (Includes Enable bit)
#define TPS55288_STATUS       0x07 // Operating Status

// Constants for VREF calculation based on datasheet page 27
#define VREF_MIN_MV 45.0f
#define VREF_STEP_MV 1.129f



class TPS55288 {
public:

  //Default values
float voltage = 5;
float current = 500;

  void print_converter();

  /**
   * @brief Constructor for the TPS55288 class.
   * @param wire The TwoWire interface to use for I2C communication.
   */
  TPS55288(TwoWire &wire = Wire);

  /**
   * @brief Initializes the I2C communication. Must be called in setup().
   */
  void begin();

  /**
   * @brief Sets the output voltage of the converter.
   * @note This function assumes the default internal feedback ratio is being used
   * (INTFB bits in VOUT_FS register = 11b), which corresponds to a 20V maximum output.
   * To use other ranges or external feedback, you must configure the VOUT_FS register (0x04) separately.
   * @param voltage The desired output voltage in volts (e.g., 5.0, 12.0).
   * @return True if the voltage was set successfully, false otherwise.
   */
  bool setVoltage(float voltage);

  /**
   * @brief Sets the output current limit settings. Low precision, value gets rounded to cca 50 mA steps
   * @param current value in mA
   */
  bool setCurrentLimit(float current);

  void update(){
    setVoltage(voltage);
    setCurrentLimit(current);
  }

  /**
   * @brief Enables the converter's output.
   * @return True if the output was enabled successfully, false otherwise.
   */
  bool enable();

  /**
   * @brief Disables the converter's output.
   * @return True if the output was disabled successfully, false otherwise.
   */
  bool disable();

  /**
   * @brief Reads the status register (0x07) of the converter.
   * @return The 8-bit value of the status register. Returns 0xFF on I2C read error.
   */
  uint8_t getStatus();
  bool enabled = false;

private:
  TwoWire* _wire;
  uint8_t _deviceAddress;

  /**
   * @brief Writes an 8-bit value to a specific register.
   * @param reg The 8-bit register address to write to.
   * @param value The 8-bit value to write.
   * @return True if the write was successful, false otherwise.
   */
  bool writeRegister(uint8_t reg, uint8_t value);

  /**
   * @brief Reads an 8-bit value from a specific register.
   * @param reg The 8-bit register address to read from.
   * @param value A reference to a uint8_t variable to store the read value.
   * @return True if the read was successful, false otherwise.
   */
  bool readRegister(uint8_t reg, uint8_t &value);
};

#endif // CONVERTER_H
