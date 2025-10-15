#ifndef CHARGER_H
#define CHARGER_H

#include <Arduino.h>
#include <Wire.h>

#define BQ25703A_ADDRESS 0x6B

// Register Addresses
#define CHARGE_OPTION_0_REG 0x00
#define MAX_CHARGE_VOLTAGE_REG 0x04
#define CHARGE_CURRENT_REG 0x02
#define CHARGE_OPTION_1_REG 0x30
#define CHARGE_OPTION_2_REG 0x32
#define CHARGE_OPTION_3_REG 0x34
#define ADC_OPTION_REG 0x3A
#define CHARGER_STATUS_REG 0x20
#define PROCHOT_STATUS_REG 0x22
#define IIN_DPM_REG 0x24
#define ADC_VBUS_PSYS_REG 0x26
#define ADC_IBAT_REG 0x28
#define ADC_IIN_CMPIN_REG 0x2A
#define ADC_VSYS_VBAT_REG 0x2C
#define OTG_VOLTAGE_REG 0x06
#define OTG_CURRENT_REG 0x08
#define INPUT_VOLTAGE_REG 0x0A
#define MIN_SYSTEM_VOLTAGE_REG 0x0C
#define IIN_HOST_REG 0x0E
#define MANUFACTURER_ID_REG 0x2E
#define DEVICE_ID_REG 0x2F

// Bitmasks for CHARGE_OPTION_0_REG (0x00)
#define CHARGE_OPTION_0_DEFAULT     0x820E
#define CHG_OPT0_EN_LWPWR           (1 << 15) // Enable low power mode
#define CHG_OPT0_WDTMR_ADJ_DISABLE  (0 << 13)
#define CHG_OPT0_EN_OOA             (1 << 10) // Out of audio switch frequency
#define CHG_OPT0_FREQ_HIGH          (0 << 9)

// Bitmasks for CHARGE_OPTION_1_REG (0x30)
#define CHARGE_OPTION_1_DEFAULT     0x0211
#define CHG_OPT1_EN_IBAT            (1 << 15) // Enable IBAT output buffer
#define CHG_OPT1_EN_PSYS            (1 << 12) // Enable PSYS buffer

// Bitmasks for CHARGE_OPTION_2_REG (0x32)
#define CHARGE_OPTION_2_DEFAULT     0x02B7
#define CHG_OPT2_EN_EXTILIM         (1 << 15) // Allow ILIM_HIZ pin to set current limit

// Bitmasks for ADC_OPTION_REG (0x3A)
#define ADC_OPTION_DEFAULT          0x2000
#define ADC_OPT_ADC_CONV            (1 << 15) // 1 = continuous conversion
#define ADC_OPT_ADC_START           (1 << 14) // Start one-shot conversion
#define ADC_OPT_EN_ADC_VBUS         (1 << 6)
#define ADC_OPT_EN_ADC_PSYS         (1 << 5)
#define ADC_OPT_EN_ADC_IDCHG        (1 << 3)
#define ADC_OPT_EN_ADC_ICHG         (1 << 2)
#define ADC_OPT_EN_ADC_VSYS         (1 << 1)
#define ADC_OPT_EN_ADC_VBAT         (1 << 0)

// Function prototypes for simplified I2C communication
void write_register(uint8_t reg, uint16_t value);
uint16_t read_register(uint8_t reg);

void bq25703aRegisterDump();

void setup_charger();

void handle_charger();

void print_charger();

//reads the charger values and returns via pointers
void read_charger(uint16_t* vbus_mv, uint16_t* vbat_mv, uint16_t* vsys_mv, uint16_t* ichg_ma);

#endif // CHARGER_H