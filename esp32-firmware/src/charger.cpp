#include "charger.h"

// BQ25703A I2C Address (7-bit)
const uint8_t BQ25703A_I2C_ADDR = 0x6B;

// --- BQ25703A Register Addresses (Word Access - 16 bit) ---
// Addresses are for the LSB of the 16-bit word.
const uint8_t REG_CHARGE_OPTION_0       = 0x00;
const uint8_t REG_CHARGE_CURRENT        = 0x02;
const uint8_t REG_CHARGE_VOLTAGE        = 0x04;
// const uint8_t REG_OTG_VOLTAGE           = 0x06; // Not used in this basic example
// const uint8_t REG_OTG_CURRENT           = 0x08; // Not used in this basic example
const uint8_t REG_INPUT_VOLTAGE_LIMIT   = 0x0A; // InputVoltageLimit (VINDPM threshold)
const uint8_t REG_MIN_SYSTEM_VOLTAGE    = 0x0C;
const uint8_t REG_INPUT_CURRENT_LIMIT   = 0x0E; // InputCurrentLimit (IIN_HOST / IINDPM)
const uint8_t REG_CHARGE_STATUS         = 0x20;
const uint8_t REG_PROCHOT_STATUS        = 0x22;
const uint8_t REG_INPUT_VOLTAGE_ADC     = 0x27; // ReadInputVoltageADC (actually Input Voltage, not current)
const uint8_t REG_INPUT_CURRENT_ADC     = 0x28; // ReadInputCurrentADC (Actual input current IIN_ADC)
const uint8_t REG_VBAT_ADC              = 0x29;
const uint8_t REG_VSYS_ADC              = 0x2B;
const uint8_t REG_CHARGE_OPTION_1       = 0x30;
// const uint8_t REG_CHARGE_OPTION_2       = 0x32;
// const uint8_t REG_CHARGE_OPTION_3       = 0x34;
// const uint8_t REG_PROCHOT_OPTION_0      = 0x36;
// const uint8_t REG_PROCHOT_OPTION_1      = 0x38;
const uint8_t REG_ADC_OPTION            = 0x3A;
const uint8_t REG_MANUFACTURER_ID       = 0x2E;
const uint8_t REG_DEVICE_ID             = 0x2F;

// Expected Device IDs
const uint16_t EXPECTED_MANUFACTURER_ID = 0x0040; // Texas Instruments
const uint16_t EXPECTED_DEVICE_ID       = 0x000B; // BQ25703A

void bq25703a_write_register(uint8_t reg_addr, uint16_t value) {
    Wire.beginTransmission(BQ25703A_I2C_ADDR);
    Wire.write(reg_addr);
    Wire.write(value & 0xFF); // LSB
    Wire.write(value >> 8);   // MSB
    if (Wire.endTransmission() != 0) {
        Serial.print(F("Error writing to BQ25703A register 0x"));
        Serial.println(reg_addr, HEX);
    }
}

uint16_t bq25703a_read_register(uint8_t reg_addr) {
    uint16_t value = 0xFFFF; // Default to error value

    Wire.beginTransmission(BQ25703A_I2C_ADDR);
    Wire.write(reg_addr);
    if (Wire.endTransmission(false) != 0) { // Send address, false = restart for read
        Serial.print(F("Error setting read pointer for BQ25703A register 0x"));
        Serial.println(reg_addr, HEX);
        return value;
    }

    if (Wire.requestFrom(BQ25703A_I2C_ADDR, (uint8_t)2) == 2) {
        uint8_t lsb = Wire.read();
        uint8_t msb = Wire.read();
        value = (uint16_t)msb << 8 | lsb;
    } else {
        Serial.print(F("Error reading from BQ25703A register 0x"));
        Serial.println(reg_addr, HEX);
    }
    return value;
}

bool bq25703a_check_device() {
    uint16_t manID = bq25703a_read_register(REG_MANUFACTURER_ID);
    uint16_t devID = bq25703a_read_register(REG_DEVICE_ID);

    if (manID == EXPECTED_MANUFACTURER_ID && devID == EXPECTED_DEVICE_ID) {
        return true;
    }
    Serial.print(F("BQ25703A Mismatch! ManID: 0x")); Serial.print(manID, HEX);
    Serial.print(F(" DevID: 0x")); Serial.println(devID, HEX);
    Serial.print(F("Expected ManID: 0x")); Serial.print(EXPECTED_MANUFACTURER_ID, HEX);
    Serial.print(F(" ExpDevID: 0x")); Serial.println(EXPECTED_DEVICE_ID, HEX);
    return false;
}

bool bq25703a_init(int8_t sda_pin, int8_t scl_pin, uint32_t i2c_frequency) {

    return bq25703a_check_device();
}

bool bq25703a_configure_for_2s(float charge_voltage_V,
                               float charge_current_A,
                               float input_current_limit_A,
                               float min_system_voltage_V,
                               float vindpm_offset_V) {
    Serial.println(F("Configuring BQ25703A for 2S Li-ion..."));

    // 1. Disable Watchdog Timer (WDT_EN = 0 in ChargeOption0)
    uint16_t chargeOption0 = bq25703a_read_register(REG_CHARGE_OPTION_0);
    if (chargeOption0 == 0xFFFF) return false; // Read error
    chargeOption0 &= ~(1 << 6); // Clear bit 6 (WDT_EN = 0)
    // Explicitly set cell count to 2S (bits 2:1 = 01)
    chargeOption0 &= ~((1<<2) | (1<<1)); // Clear bits 2:1
    chargeOption0 |= (0b01 << 1);        // Set for 2S
    bq25703a_write_register(REG_CHARGE_OPTION_0, chargeOption0);
    Serial.print(F("Set ChargeOption0 (Watchdog, Cells): 0x")); Serial.println(bq25703a_read_register(REG_CHARGE_OPTION_0), HEX);

    // 2. Set Charge Voltage (REG_CHARGE_VOLTAGE)
    // Resolution: 8 mV/LSb.
    uint16_t chargeVoltageRegVal = (uint16_t)(charge_voltage_V * 1000 / 8);
    bq25703a_write_register(REG_CHARGE_VOLTAGE, chargeVoltageRegVal);
    Serial.print(F("Set Charge Voltage to: ")); Serial.print(charge_voltage_V);
    Serial.print(F("V (Reg: 0x")); Serial.print(chargeVoltageRegVal, HEX);
    Serial.print(F("), Readback: 0x")); Serial.println(bq25703a_read_register(REG_CHARGE_VOLTAGE), HEX);

    // 3. Set Charge Current (REG_CHARGE_CURRENT)
    // Resolution: 64 mA/LSb.
    uint16_t chargeCurrentRegVal = (uint16_t)(charge_current_A * 1000 / 64);
    bq25703a_write_register(REG_CHARGE_CURRENT, chargeCurrentRegVal);
    Serial.print(F("Set Charge Current to: ")); Serial.print(charge_current_A);
    Serial.print(F("A (Reg: 0x")); Serial.print(chargeCurrentRegVal, HEX);
    Serial.print(F("), Readback: 0x")); Serial.println(bq25703a_read_register(REG_CHARGE_CURRENT), HEX);

    // 4. Set Input Current Limit (IIN_HOST) (REG_INPUT_CURRENT_LIMIT)
    // Resolution: 50 mA/LSb.
    uint16_t inputCurrentLimitRegVal = (uint16_t)(input_current_limit_A * 1000 / 50);
    bq25703a_write_register(REG_INPUT_CURRENT_LIMIT, inputCurrentLimitRegVal);
    Serial.print(F("Set Input Current Limit to: ")); Serial.print(input_current_limit_A);
    Serial.print(F("A (Reg: 0x")); Serial.print(inputCurrentLimitRegVal, HEX);
    Serial.print(F("), Readback: 0x")); Serial.println(bq25703a_read_register(REG_INPUT_CURRENT_LIMIT), HEX);

    // 5. Set Minimum System Voltage (REG_MIN_SYSTEM_VOLTAGE)
    // Resolution: 256 mV/LSb.
    uint16_t minSystemVoltageRegVal = (uint16_t)(min_system_voltage_V * 1000 / 256);
    bq25703a_write_register(REG_MIN_SYSTEM_VOLTAGE, minSystemVoltageRegVal);
    Serial.print(F("Set Min System Voltage to: ")); Serial.print(min_system_voltage_V);
    Serial.print(F("V (Reg: 0x")); Serial.print(minSystemVoltageRegVal, HEX);
    Serial.print(F("), Readback: 0x")); Serial.println(bq25703a_read_register(REG_MIN_SYSTEM_VOLTAGE), HEX);

    // 6. Set Input Voltage Regulation Threshold (VINDPM Offset) (REG_INPUT_VOLTAGE_LIMIT)
    // Relative offset: bits 4-0, resolution 128mV. Bit 15 = 0 for relative.
    uint16_t vindpmOffsetValBits = (uint16_t)(vindpm_offset_V * 1000 / 128);
    uint16_t inputVoltageLimitReg = bq25703a_read_register(REG_INPUT_VOLTAGE_LIMIT);
    if (inputVoltageLimitReg == 0xFFFF) return false;
    inputVoltageLimitReg &= ~((1 << 15) | 0x001F); // Clear bit 15 (relative mode) and bits 4-0
    inputVoltageLimitReg |= (vindpmOffsetValBits & 0x001F);
    bq25703a_write_register(REG_INPUT_VOLTAGE_LIMIT, inputVoltageLimitReg);
    Serial.print(F("Set VINDPM Offset (Relative) to: ")); Serial.print(vindpm_offset_V);
    Serial.print(F("V (Offset bits: 0x")); Serial.print(vindpmOffsetValBits, HEX);
    Serial.print(F("), Full Reg: 0x")); Serial.print(inputVoltageLimitReg, HEX);
    Serial.print(F("), Readback: 0x")); Serial.println(bq25703a_read_register(REG_INPUT_VOLTAGE_LIMIT), HEX);

    // 7. Enable ADC for monitoring (REG_ADC_OPTION)
    // Bit 15: ADC_EN (1=enable continuous).
    uint16_t adcOption = bq25703a_read_register(REG_ADC_OPTION);
    if (adcOption == 0xFFFF) return false;
    adcOption |= (1 << 15); // Enable ADC (continuous mode)
    bq25703a_write_register(REG_ADC_OPTION, adcOption);
    Serial.print(F("Set ADCOption to enable ADC: 0x")); Serial.println(bq25703a_read_register(REG_ADC_OPTION), HEX);

    // Charge is enabled/disabled by bq25703a_enable_charging()
    // By default, ensure CHG_INHIBIT is cleared after configuration.
    bq25703a_enable_charging(true);

    Serial.println(F("BQ25703A Configuration for 2S complete."));
    return true;
}

void bq25703a_enable_charging(bool enable) {
    uint16_t chargeOption1 = bq25703a_read_register(REG_CHARGE_OPTION_1);
    if (chargeOption1 == 0xFFFF) return; // Read error

    if (enable) {
        chargeOption1 &= ~(1 << 0); // Clear bit 0 (CHG_INHIBIT = 0, enable charging)
        Serial.println(F("Enabling charging."));
    } else {
        chargeOption1 |= (1 << 0);  // Set bit 0 (CHG_INHIBIT = 1, inhibit charging)
        Serial.println(F("Disabling charging."));
    }
    bq25703a_write_register(REG_CHARGE_OPTION_1, chargeOption1);
    Serial.print(F("Set ChargeOption1: 0x")); Serial.println(bq25703a_read_register(REG_CHARGE_OPTION_1), HEX);
}

void bq25703a_print_status() {
    uint16_t chargeStatus = bq25703a_read_register(REG_CHARGE_STATUS);
    uint16_t prochotStatus = bq25703a_read_register(REG_PROCHOT_STATUS);

    Serial.println(F("\n--- BQ25703A Status ---"));
    if (chargeStatus == 0xFFFF) {
        Serial.println(F("Failed to read charger status."));
        return;
    }
    Serial.print(F("ChargeStatus (0x20): 0x")); Serial.println(chargeStatus, HEX);
    Serial.print(F("  VBUS Present (bit15): ")); Serial.println((chargeStatus >> 15) & 0x01 ? "Yes" : "No");
    // Bits 12:11 are CHRG_STAT in BQ25703A datasheet (Section 8.6.14)
    uint8_t chrgStatBits = (chargeStatus >> 11) & 0x03;
    Serial.print(F("  Charging State (bits 12:11): "));
    switch(chrgStatBits) {
        case 0b00: Serial.println(F("Not Charging / Charge Termination")); break;
        case 0b01: Serial.println(F("Trickle Charge")); break;
        case 0b10: Serial.println(F("Pre-charge / Fast Charge (CC)")); break;
        case 0b11: Serial.println(F("Constant Voltage (CV - Taper)")); break;
        default: Serial.println(F("Unknown")); break;
    }

    Serial.print(F("ProchotStatus (0x22): 0x")); Serial.println(prochotStatus, HEX);

    // ADC Readings
    uint16_t rawInputCurrent = bq25703a_read_register(REG_INPUT_CURRENT_ADC); // IIN_ADC 0x28
    float inputCurrentA = rawInputCurrent * 0.050f; // Resolution 50mA/LSb

    uint16_t rawVBat = bq25703a_read_register(REG_VBAT_ADC); // VBAT_ADC 0x29
    uint16_t rawVSys = bq25703a_read_register(REG_VSYS_ADC); // VSYS_ADC 0x2B

    uint16_t chgOpt0 = bq25703a_read_register(REG_CHARGE_OPTION_0);
    float batteryVoltageV, systemVoltageV;

    // VBAT ADC: bit15 of ChargeOption0 is VBAT_REGN_DIS
    if ((chgOpt0 >> 15) & 0x01) { // VBAT_REGN_DIS = 1
         batteryVoltageV = rawVBat * 0.008f; // 8mV/LSb, 0mV offset
    } else { // VBAT_REGN_DIS = 0 (default)
         batteryVoltageV = (rawVBat * 64.0f + 2816.0f) / 1000.0f; // 64mV/LSb, 2816mV offset
    }

    // VSYS ADC: bit14 of ChargeOption0 is VSYS_REGN_DIS
    if ((chgOpt0 >> 14) & 0x01) { // VSYS_REGN_DIS = 1
         systemVoltageV = rawVSys * 0.008f; // 8mV/LSb, 0mV offset
    } else { // VSYS_REGN_DIS = 0 (default)
         systemVoltageV = (rawVSys * 64.0f + 2816.0f) / 1000.0f; // 64mV/LSb, 2816mV offset
    }
    
    uint16_t rawInputVoltage = bq25703a_read_register(REG_INPUT_VOLTAGE_ADC); // VBUS_ADC 0x27
    // VBUS_ADC (InputVoltageADC): Resolution 64mV, Offset 3.2V
    float inputVoltageV = (rawInputVoltage * 64.0f + 3200.0f) / 1000.0f;


    Serial.print(F("  Input Voltage (ADC 0x27): ")); Serial.print(inputVoltageV, 3); Serial.println(F(" V"));
    Serial.print(F("  Input Current (ADC 0x28): ")); Serial.print(inputCurrentA, 3); Serial.println(F(" A"));
    Serial.print(F("  Battery Voltage (ADC 0x29): ")); Serial.print(batteryVoltageV, 3); Serial.println(F(" V"));
    Serial.print(F("  System Voltage (ADC 0x2B): ")); Serial.print(systemVoltageV, 3); Serial.println(F(" V"));
    Serial.println(F("-------------------------"));
}