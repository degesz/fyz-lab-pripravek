#include "charger.h"

// Helper function to write a 16-bit value to a register
void write_register(uint8_t reg, uint16_t value) {
  Wire.beginTransmission(BQ25703A_ADDRESS);
  Wire.write(reg);
  Wire.write(value & 0xFF); // LSB
  Wire.write(value >> 8);   // MSB
  Wire.endTransmission();
}

// Helper function to read a 16-bit value from a register
uint16_t read_register(uint8_t reg) {
  Wire.beginTransmission(BQ25703A_ADDRESS);
  Wire.write(reg);
  Wire.endTransmission(false);

  Wire.requestFrom((uint8_t)BQ25703A_ADDRESS, (uint8_t)2);
  if (Wire.available() >= 2) {
    uint8_t lsb = Wire.read();
    uint8_t msb = Wire.read();
    return (msb << 8) | lsb;
  }
  return 0;
}

void setup_charger() {
  Wire.beginTransmission(BQ25703A_ADDRESS);
  byte error = Wire.endTransmission();

  if (error == 0) {
    // Set minimum system voltage to 6.144V
    write_register(MIN_SYSTEM_VOLTAGE_REG, 6144);
    delay(15);

    // ChargeOption0: Disable watchdog, disable LWPWR, enable OOA
    uint16_t charge_option_0 = CHARGE_OPTION_0_DEFAULT;
    charge_option_0 &= ~CHG_OPT0_EN_LWPWR; // Disable low power mode
    charge_option_0 |= CHG_OPT0_WDTMR_ADJ_DISABLE;
    charge_option_0 |= CHG_OPT0_EN_OOA;
    charge_option_0 |= CHG_OPT0_FREQ_HIGH;
    write_register(CHARGE_OPTION_0_REG, charge_option_0);
    delay(15);

    // ChargeOption2: Disable external ILIM
    uint16_t charge_option_2 = CHARGE_OPTION_2_DEFAULT;
    charge_option_2 &= ~CHG_OPT2_EN_EXTILIM;
    write_register(CHARGE_OPTION_2_REG, charge_option_2);
    delay(15);
    
    // ChargeOption1: Enable IBAT and PSYS measurements
    uint16_t charge_option_1 = CHARGE_OPTION_1_DEFAULT;
    charge_option_1 |= CHG_OPT1_EN_IBAT;
    charge_option_1 |= CHG_OPT1_EN_PSYS;
    write_register(CHARGE_OPTION_1_REG, charge_option_1);
    delay(15);

    // ADCOption: Enable continuous conversion and select ADC channels
    uint16_t adc_option = ADC_OPTION_DEFAULT;
    adc_option |= ADC_OPT_ADC_CONV;
    adc_option |= ADC_OPT_EN_ADC_VBUS;
    adc_option |= ADC_OPT_EN_ADC_PSYS;
    adc_option |= ADC_OPT_EN_ADC_IDCHG;
    adc_option |= ADC_OPT_EN_ADC_ICHG;
    adc_option |= ADC_OPT_EN_ADC_VSYS;
    adc_option |= ADC_OPT_EN_ADC_VBAT;
    write_register(ADC_OPTION_REG, adc_option);
    delay(15);

    // ADCOption: Start ADC conversion
    adc_option |= ADC_OPT_ADC_START;
    write_register(ADC_OPTION_REG, adc_option);
    delay(15);

    // Set max charge voltage to 8.380V (rounded to 8.368V)
    write_register(MAX_CHARGE_VOLTAGE_REG, 8368);
    delay(15);

    // Set charge current to 1000mA (rounded to 960mA)
    write_register(CHARGE_CURRENT_REG, 960);

  } else {
    Serial.println("Charger not connected, stopping program.");
    while (1);
  }
}

void handle_charger() {
  // This function can be used for periodic charger tasks
}

void read_charger(uint16_t* vbus_mv, uint16_t* vbat_mv, uint16_t* vsys_mv, uint16_t* ichg_ma) {
    uint16_t vbus_psys = read_register(ADC_VBUS_PSYS_REG);
    uint16_t vsys_vbat = read_register(ADC_VSYS_VBAT_REG);
    uint16_t ibat      = read_register(ADC_IBAT_REG);

    *vbus_mv = ((vbus_psys >> 8) * 64) + 3200;
    *vbat_mv = ((vsys_vbat & 0xFF) * 64) + 2880;
    *vsys_mv = ((vsys_vbat >> 8) * 64) + 2880;
    *ichg_ma = (ibat >> 8) * 64;
}


void print_charger() {
  uint16_t vbus_psys = read_register(ADC_VBUS_PSYS_REG);
  uint16_t vsys_vbat = read_register(ADC_VSYS_VBAT_REG);
  uint16_t ibat =      read_register(ADC_IBAT_REG);

  uint16_t vbus_mv = ((vbus_psys >> 8) * 64) + 3200;
  uint16_t vbat_mv = ((vsys_vbat & 0xFF) * 64) + 2880;
  uint16_t vsys_mv = ((vsys_vbat >> 8) * 64) + 2880;
  uint16_t ichg_ma = (ibat >> 8) * 64;

  Serial.print("Voltage of VBUS: ");
  Serial.print(vbus_mv);
  Serial.println("mV");

  Serial.print("Voltage of VBAT: ");
  Serial.print(vbat_mv);
  Serial.println("mV");

  Serial.print("Voltage of VSYS: ");
  Serial.print(vsys_mv);
  Serial.println("mV");

  Serial.print("Charging current: ");
  Serial.print(ichg_ma);
  Serial.println("mA");
}

const uint8_t bq25703aRegisters[] = {
  CHARGE_OPTION_0_REG,
  MAX_CHARGE_VOLTAGE_REG,
  CHARGE_CURRENT_REG,
  CHARGE_OPTION_1_REG,
  CHARGE_OPTION_2_REG,
  CHARGE_OPTION_3_REG,
  ADC_OPTION_REG,
  CHARGER_STATUS_REG,
  PROCHOT_STATUS_REG,
  IIN_DPM_REG,
  ADC_VBUS_PSYS_REG,
  ADC_IBAT_REG,
  ADC_IIN_CMPIN_REG,
  ADC_VSYS_VBAT_REG,
  OTG_VOLTAGE_REG,
  OTG_CURRENT_REG,
  INPUT_VOLTAGE_REG,
  MIN_SYSTEM_VOLTAGE_REG,
  IIN_HOST_REG,
  MANUFACTURER_ID_REG,
  DEVICE_ID_REG
};

void bq25703aRegisterDump() {
  Serial.println("\n--- BQ25703A Register Dump ---");

  for (size_t i = 0; i < sizeof(bq25703aRegisters) / sizeof(bq25703aRegisters[0]); ++i) {
    uint8_t regAddress = bq25703aRegisters[i];
    uint16_t regValue = read_register(regAddress);

    Serial.print("Reg 0x");
    if (regAddress < 0x10) Serial.print("0");
    Serial.print(regAddress, HEX);
    Serial.print(": 0x");
    if (regValue < 0x1000) Serial.print("0");
    if (regValue < 0x100) Serial.print("0");
    if (regValue < 0x10) Serial.print("0");
    Serial.println(regValue, HEX);
    delay(10);
  }
  Serial.println("--- Register Dump Complete ---");
}