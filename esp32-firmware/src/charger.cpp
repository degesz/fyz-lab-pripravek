#include "charger.h"
#define BQ25703ADevaddr    0x6B


const byte Lorro_BQ25703A::BQ25703Aaddr = BQ25703ADevaddr;

Lorro_BQ25703A BQ25703A;
//Instantiate with reference to global set
Lorro_BQ25703A::Regt BQ25703Areg;

void setup_charger(){
 Wire.beginTransmission(0x6B);
  byte error = Wire.endTransmission();

  if (error == 0) {
   // Serial.println("Charger found!");


BQ25703Areg.minSystemVoltage.set_voltage(6144);
   BQ25703A.writeRegEx( BQ25703Areg.minSystemVoltage );
  delay( 15 );

  //Set the watchdog timer to not have a timeout
  BQ25703Areg.chargeOption0.set_WDTMR_ADJ( 0 );
  BQ25703Areg.chargeOption0.set_EN_LWPWR( 0 );
  BQ25703Areg.chargeOption0.set_IDPM_AUTO_DISABLE( 0 );
  BQ25703Areg.chargeOption0.set_EN_OOA( 1 );
  BQ25703A.writeRegEx( BQ25703Areg.chargeOption0 );
  delay( 15 );

  BQ25703Areg.chargeOption2.set_EN_EXTILIM(0);
  BQ25703A.writeRegEx( BQ25703Areg.chargeOption2 );
  delay( 15 );
  //Set the ADC on IBAT and PSYS to record values
  //When changing bitfield values, call the writeRegEx function
  //This is so you can change all the bits you want before sending out the byte.
  BQ25703Areg.chargeOption1.set_EN_IBAT( 1 );
  BQ25703Areg.chargeOption1.set_EN_PSYS( 1 );
  BQ25703A.writeRegEx( BQ25703Areg.chargeOption1 );
  delay( 15 );

  //Set ADC to make continuous readings. (uses more power)
  delay( 15 );
  BQ25703Areg.aDCOption.set_ADC_CONV( 1 );
    BQ25703A.writeRegEx( BQ25703Areg.aDCOption );
  //Set individual ADC registers to read. All have default off.
  BQ25703Areg.aDCOption.set_EN_ADC_VBUS( 1 );
  BQ25703Areg.aDCOption.set_EN_ADC_PSYS( 1 );
  BQ25703Areg.aDCOption.set_EN_ADC_IDCHG( 1 );
  BQ25703Areg.aDCOption.set_EN_ADC_ICHG( 1 );
  BQ25703Areg.aDCOption.set_EN_ADC_VSYS( 1 );
  BQ25703Areg.aDCOption.set_EN_ADC_VBAT( 1 );
  //Once bits have been twiddled, send bytes to device
  BQ25703A.writeRegEx( BQ25703Areg.aDCOption );

  delay( 15 );
    BQ25703Areg.aDCOption.set_ADC_START( 1);
    BQ25703A.writeRegEx( BQ25703Areg.aDCOption );
  delay( 15 );



BQ25703Areg.maxChargeVoltage.set_voltage( 8400 );
  delay( 15 );
  BQ25703A.writeRegEx( BQ25703Areg.maxChargeVoltage );
  delay( 15 );
  //Sets the charge current. This needs to be set before any charging of
  //the batteries starts, as it is defaulted to 0. Any value entered will
  //be rounded to multiples of 64mA.
  BQ25703Areg.chargeCurrent.set_current( 800 );
  delay( 15 );
  BQ25703A.writeRegEx( BQ25703Areg.chargeCurrent );
  delay( 15 );







  } else {
    Serial.println("Charger not connected, stopping program.");
    while (1);
    
  }
   

}



void handle_charger(){

}

void print_charger(){
       Serial.print( "Voltage of VBUS: " );
    Serial.print( BQ25703Areg.aDCVBUSPSYS.get_VBUS() );
    Serial.println( "mV" );
    delay( 15 );

//    Serial.print( "System power usage: " );
//    Serial.print( BQ25703Areg.aDCVBUSPSYS.get_sysPower() );
//    Serial.println( "W" );
//    delay( 15 );

    Serial.print( "Voltage of VBAT: " );
    Serial.print( BQ25703Areg.aDCVSYSVBAT.get_VBAT() );
    Serial.println( "mV" );
    delay( 15 );

    Serial.print( "Voltage of VSYS: " );
    Serial.print( BQ25703Areg.aDCVSYSVBAT.get_VSYS() );
    Serial.println( "mV" );
    delay( 15 );

    Serial.print( "Charging current: " );
    Serial.print( BQ25703Areg.aDCIBAT.get_ICHG() );
    Serial.println( "mA" );
    delay( 15 );

    Serial.print( "Discharge current: " );
    Serial.print( BQ25703Areg.aDCIBAT.get_IDCHG() );
    Serial.println( "mA" );
    delay( 15 );

  Serial.println("----------------");


 // Wire.beginTransmission(BQ25703ADevaddr);
 // Wire.write(0x04); // registr k přečtení
 // Wire.endTransmission(false);    // restart pro čtení
 //
 // Wire.requestFrom(BQ25703ADevaddr, 2); // čteme 2 bajty
 // if (Wire.available() == 2) {
 //   uint8_t lsb = Wire.read();
 //   uint8_t msb = Wire.read();
 //   Serial.println(((uint16_t)msb << 8) | lsb, HEX); // spojíme MSB a LSB
 // }

}



void bq25703aRegisterDump() {
  Serial.println("\n--- BQ25703A Register Dump ---");

  // Iterate through each register address in the array
  for (size_t i = 0; i < sizeof(bq25703aRegisters) / sizeof(bq25703aRegisters[0]); ++i) {
    uint8_t regAddress = bq25703aRegisters[i];
    uint16_t regValue = 0; // Variable to store the 16-bit register value

    // Begin transmission to the BQ25703A device
    Wire.beginTransmission(0x6B);
    // Write the register address we want to read from
    Wire.write(regAddress);
    // End transmission with 'false' to send a repeated start condition for reading
    // This keeps the connection open for the subsequent read request.
    byte status = Wire.endTransmission(false);

    if (status != 0) {
      Serial.print("Error writing to register 0x");
      if (regAddress < 0x10) Serial.print("0"); // Pad with leading zero for single digit hex
      Serial.print(regAddress, HEX);
      Serial.print(": ");
      Serial.println(status == 1 ? "Data too long to fit in transmit buffer" :
                     status == 2 ? "Received NACK on transmit of address" :
                     status == 3 ? "Received NACK on transmit of data" :
                     status == 4 ? "Other error" : "Unknown error");
      continue; // Skip to the next register if there was a write error
    }

    // Request 2 bytes from the device, starting from the previously specified register address
    Wire.requestFrom(0x6B, 2);

    // Check if 2 bytes were successfully received
    if (Wire.available() == 2) {
      uint8_t lsb = Wire.read(); // Read the Least Significant Byte (LSB)
      uint8_t msb = Wire.read(); // Read the Most Significant Byte (MSB)

      // Combine MSB and LSB to form the 16-bit value
      regValue = ((uint16_t)msb << 8) | lsb;

      // Print the register address and its value to the Serial monitor
      Serial.print("Reg 0x");
      if (regAddress < 0x10) Serial.print("0"); // Pad with leading zero for single digit hex
      Serial.print(regAddress, HEX);
      Serial.print(": 0x");
      if (regValue < 0x1000) Serial.print("0"); // Pad with leading zeros for 16-bit hex
      if (regValue < 0x100) Serial.print("0");
      if (regValue < 0x10) Serial.print("0");
      Serial.println(regValue, HEX);
    } else {
      // Handle cases where not enough bytes were received
      Serial.print("Failed to read 2 bytes from register 0x");
      if (regAddress < 0x10) Serial.print("0");
      Serial.println(regAddress, HEX);
    }
    delay(10); // Small delay between reads to ensure stability
  }
  Serial.println("--- Register Dump Complete ---");
}