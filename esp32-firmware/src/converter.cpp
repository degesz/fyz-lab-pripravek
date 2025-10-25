#include "converter.h"

/**
 * @brief Constructor for the TPS55288 class.
 */
TPS55288::TPS55288(TwoWire &wire)
{
  _wire = &wire;
  _deviceAddress = TPS55288_I2C_ADDRESS;
}

/**
 * @brief Initializes the I2C communication.
 */
//void TPS55288::begin()
//{
//
//  uint8_t mode_value;
//
//  mode_value |= (0 << 7); // Set bit 7 (OE - Output Enable) to 0
//  mode_value |= (0 << 6); // Set bit 6 (frequency doubling) to 0
//  mode_value |= (1 << 5); // Set bit 5 (Hiccup mode) to 1
//  mode_value |= (0 << 4); // Set bit 4 (output discharge) to 0
//  mode_value |= (0 << 3); // Set bit 3 (vcc source 0=internal 1 = external) to 0
//  mode_value |= (1 << 2); // Set bit 2 (I2C addr 1 = 0x75)  to 1
//  mode_value |= (1 << 1); // Set bit 1 (0=PFM, 1=PWM) to 1
//  mode_value |= (1 << 0); // Set bit 0 (mode setting 1 = software) to 1
//
//  Serial.print("setup reg: ");
//  Serial.println(mode_value, BIN);
//  if (!writeRegister(TPS55288_MODE, mode_value))
//  {
//    Serial.println("Converter begin fail, stopping");
//    while(1);
//  }
//  
//  delay(10);
//  digitalWrite(GPIO_NUM_13, LOW);
//  return;
//}

/**
 * @brief Sets the output voltage.
 * This implementation is based on the formulas and values from the TPS55288 datasheet (slvsf01b).
 * It calculates the required internal VREF based on the desired output voltage, assuming
 * the default internal feedback setting (VOUT_FS register 0x04, INTFB bits = 11b),
 * which gives a VREF/VOUT ratio of 0.0564.
 * VREF = VOUT * 0.0564
 * The calculated VREF is then converted to a 10-bit DAC value.
 * DAC_value = (VREF_in_mV - 45mV) / 1.129mV_per_step
 */
bool TPS55288::setVoltage(float voltage)
{
  if (!enabled)
  {
    Serial.println("Convertor disabled");
    return false;
  }
  // Assume default internal feedback ratio for 20V range (see datasheet page 30, table 7-7)
  const float feedback_ratio = 0.0564f;
  float vref_voltage = voltage * feedback_ratio;

  // Convert the calculated VREF in volts to millivolts
  float vref_mv = vref_voltage * 1000.0f;

  // Calculate the 10-bit DAC value based on the formula derived from datasheet page 27
  uint16_t dac_value = 0;
  if (vref_mv > VREF_MIN_MV)
  {
    dac_value = (uint16_t)roundf((vref_mv - VREF_MIN_MV) / VREF_STEP_MV);
  }

  // Clamp the value to the 10-bit range (0-1023)
  if (dac_value > 1023)
  {
    dac_value = 1023;
  }

  // Split the 10-bit value into two bytes for the REF registers
  uint8_t ref_lsb = dac_value & 0xFF;        // Lower 8 bits go to register 0x00
  uint8_t ref_msb = (dac_value >> 8) & 0x03; // Upper 2 bits go to register 0x01

  // Write the values to the registers.
  // Per datasheet (p. 27), writing to 0x01 triggers the update. So we write LSB then MSB.
  bool success = writeRegister(TPS55288_REF_LSB, ref_lsb);
  success &= writeRegister(TPS55288_REF_MSB, ref_msb);

  return success;
}

bool TPS55288::setCurrentLimit(float current)
{
  if (!enabled)
  {
    Serial.println("Convertor disabled");
    return false;
  }
  uint8_t current_limit_register_value = (current + 56) / 44;

  // Ensure highest bit (bit 7) is always set to enable the current limiting
  current_limit_register_value |= (1 << 7);
  Serial.print("Setting current limit:");
  Serial.println(current_limit_register_value);
  // Writes the modified 8-bit value to the IOUT_LIMIT register (0x02)
  return writeRegister(TPS55288_IOUT_LIMIT, current_limit_register_value);
}

/**
 * @brief Enables the converter output by setting the OE bit in the MODE register.
 */
bool TPS55288::enable()
{

 lv_obj_set_state(objects.output_enable, LV_STATE_CHECKED, true);
 lv_label_set_text(objects.output_enable, "ON ");

  enabled = true;
  digitalWrite(GPIO_NUM_13, HIGH);
  delay(10);
  uint8_t mode_value;
  // Read the current value of the MODE register (0x06)
  if (!readRegister(TPS55288_MODE, mode_value))
  {
    return false; // I2C read failed
  }
  

  mode_value |= (1 << 7); // Set bit 7 (OE - Output Enable) to 0
  mode_value |= (0 << 6); // Set bit 6 (frequency doubling) to 0
  mode_value |= (1 << 5); // Set bit 5 (Hiccup mode) to 1
  mode_value |= (0 << 4); // Set bit 4 (output discharge) to 0
  mode_value |= (0 << 3); // Set bit 3 (vcc source 0=internal 1 = external) to 0
  mode_value |= (1 << 2); // Set bit 2 (I2C addr 1 = 0x75)  to 1
  mode_value |= (1 << 1); // Set bit 1 (0=PFM, 1=PWM) to 1
  mode_value |= (1 << 0); // Set bit 0 (mode setting 1 = software) to 1


  // Write the modified value back to the MODE register
  return writeRegister(TPS55288_MODE, mode_value);
  
}

/**
 * @brief Disables the converter output by clearing the OE bit in the MODE register.
 */
bool TPS55288::disable()
{
  enabled = false;

  lv_obj_set_state(objects.output_enable, LV_STATE_CHECKED, false);
  lv_label_set_text(objects.output_enable, "OFF");

  uint8_t mode_value;
  // Read the current value of the MODE register (0x06)
  if (!readRegister(TPS55288_MODE, mode_value))
  {
    return false; // I2C read failed
  }

  // Clear bit 7 (OE - Output Enable) to 0
  mode_value &= ~(1 << 7);
  bool result = writeRegister(TPS55288_MODE, mode_value);
  // Write the modified value back to the MODE register
  digitalWrite(GPIO_NUM_13, LOW);
  return result;
}

/**
 * @brief Reads the status register.
 */
uint8_t TPS55288::getStatus()
{
  uint8_t status_val = 0;
  // Read from the correct STATUS register address (0x07)
  if (readRegister(TPS55288_STATUS, status_val))
  {
    return status_val;
  }
  return 0xFF; // Return 0xFF to indicate a read error
}

void TPS55288::print_converter()
{
  if (!enabled)
  {
    Serial.println("Convertor disabled");
    return;
  }

  uint8_t status = getStatus();
  switch (status & 0x03)
  {
  case 0:
    Serial.println("Mode: Boost");
    break;
  case 1:
    Serial.println("Mode: Buck");
    break;
  case 2:
    Serial.println("Mode: Buck-Boost");
    break;
  default:
    Serial.println("Mode: Reserved");
    break;
  }

  if (status & (1 << 7))
    Serial.println("SHORT CIRCUIT PROTECTION ACTIVE");
  if (status & (1 << 6))
    Serial.println("OVERCURRENT PROTECTION ACTIVE");
  if (status & (1 << 5))
    Serial.println("OVERVOLTAGE PROTECTION ACTIVE");

}

/**
 * @brief Writes a byte to a register.
 */
bool TPS55288::writeRegister(uint8_t reg, uint8_t value)
{
  _wire->beginTransmission(_deviceAddress);
  _wire->write(reg);
  _wire->write(value);
  return _wire->endTransmission() == 0;
}

/**
 * @brief Reads a byte from a register.
 */
bool TPS55288::readRegister(uint8_t reg, uint8_t &value)
{
  _wire->beginTransmission(_deviceAddress);
  _wire->write(reg);
  if (_wire->endTransmission(false) != 0)
  { // Send restart condition
    return false;
  }

  if (_wire->requestFrom((uint8_t)_deviceAddress, (uint8_t)1) != 1)
  {
    return false;
  }

  value = _wire->read();
  return true;
}
