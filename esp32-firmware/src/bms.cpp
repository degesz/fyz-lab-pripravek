#include "bms.h"


// Funkce pro čtení 16bitové hodnoty z registru
int16_t readBMSRegister(uint8_t command) {
    Wire.beginTransmission(BQ28Z610_ADDRESS);
    Wire.write(command);
    Wire.endTransmission(false); // false pro odeslání restartu a udržení spojení

    // Požadujeme 2 bajty dat (hodnota je 16bitová)
    if (Wire.requestFrom(BQ28Z610_ADDRESS, 2) == 2) {
        byte lowByte = Wire.read();
        byte highByte = Wire.read();
        return (int16_t)((highByte << 8) | lowByte); // Sestavení 16bitové hodnoty
    } else {
        // Chyba při čtení dat
        return -1; // Indikace chyby
    }
}

void bms_print() {
    Serial.println("--- BMS Data ---");

    // Čtení napětí
    int16_t voltage = readBMSRegister(BQ28Z610_VOLTAGE_CMD);
    if (voltage != -1) {
        Serial.print("Napětí: ");
        Serial.print(voltage);
        Serial.println(" mV");
    } else {
        Serial.println("Chyba při čtení napětí.");
    }

    // Čtení proudu
    int16_t current = readBMSRegister(BQ28Z610_CURRENT_CMD);
    if (current != -1) {
        Serial.print("Proud: ");
        Serial.print(current);
        Serial.println(" mA");
    } else {
        Serial.println("Chyba při čtení proudu.");
    }

    // Čtení vnitřní teploty
    int16_t tempInternalK = readBMSRegister(BQ28Z610_TEMP_INTERNAL_CMD);
    if (tempInternalK != -1) {
        // Teplota je hlášena v 0.1K, převedeme na Celsius
        float tempInternalC = (float)tempInternalK / 10.0 - 273.15;
        Serial.print("Vnitřní teplota: ");
        Serial.print(tempInternalC, 2); // Zobrazení s 2 desetinnými místy
        Serial.println(" °C");
    } else {
        Serial.println("Chyba při čtení vnitřní teploty.");
    }

    // Čtení teploty (obecná, často externí)
    int16_t tempK = readBMSRegister(BQ28Z610_TEMP_CMD);
    if (tempK != -1) {
        // Teplota je hlášena v 0.1K, převedeme na Celsius
        float tempC = (float)tempK / 10.0 - 273.15;
        Serial.print("Teplota: ");
        Serial.print(tempC, 2); // Zobrazení s 2 desetinnými místy
        Serial.println(" °C");
    } else {
        Serial.println("Chyba při čtení teploty.");
    }

    // Čtení nabíjecího napětí
    int16_t chargingVoltage = readBMSRegister(BQ28Z610_CHARGING_VOLTAGE_CMD);
    if (chargingVoltage != -1) {
        Serial.print("Nabíjecí napětí: ");
        Serial.print(chargingVoltage);
        Serial.println(" mV");
    } else {
        Serial.println("Chyba při čtení nabíjecího napětí.");
    }

    // Čtení nabíjecího proudu
    int16_t chargingCurrent = readBMSRegister(BQ28Z610_CHARGING_CURRENT_CMD);
    if (chargingCurrent != -1) {
        Serial.print("Nabíjecí proud: ");
        Serial.print(chargingCurrent);
        Serial.println(" mA");
    } else {
        Serial.println("Chyba při čtení nabíjecího proudu.");
    }

    Serial.println("----------------");


}






void bms_passthrough_FETs(){
    Wire.beginTransmission(BQ28Z610_ADDRESS);
  Wire.write(CONTROL_CMD);  // Command register
  Wire.write(CONTROL_SUBCMD_FETControl & 0xFF);        // LSB
  Wire.write((CONTROL_SUBCMD_FETControl >> 8) & 0xFF); // MSB
  Wire.endTransmission();

      Wire.beginTransmission(BQ28Z610_ADDRESS);
  Wire.write(CONTROL_CMD);  // Command register
  Wire.write(CONTROL_SUBCMD_ChargeFET & 0xFF);        // LSB
  Wire.write((CONTROL_SUBCMD_ChargeFET >> 8) & 0xFF); // MSB
  Wire.endTransmission();

      Wire.beginTransmission(BQ28Z610_ADDRESS);
  Wire.write(CONTROL_CMD);  // Command register
  Wire.write(CONTROL_SUBCMD_DischargeFET& 0xFF);        // LSB
  Wire.write((CONTROL_SUBCMD_DischargeFET >> 8) & 0xFF); // MSB
  Wire.endTransmission();

}
