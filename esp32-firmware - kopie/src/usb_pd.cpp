#include "usb_pd.h"


STUSB4500 usb; // STUSB4500 object

void setup_usb(){
    
  // --- STUSB4500 Initialization ---
  Serial.println(F("\nInitializing STUSB4500..."));
  if (!usb.begin(0x28)) { // Default STUSB4500 address is 0x28
    Serial.println(F("Cannot connect to STUSB4500. (USB-PD not powered)"));

  } else {
    Serial.println(F("Connected to STUSB4500!"));
    delay(100);
     usb.write(DEFAULT); // Assuming DEFAULT is defined in SparkFun_STUSB4500.h or your code

     
   usb.setPdoNumber(3); 
   Serial.println(F("STUSB4500: Set to request 9V if possible"));
   Serial.println();
   usb.setVoltage(3, 9); // Example: try to set PDO3 to 9V
   usb.softReset(); // This forces re-negotiation
   usb.write();

  }


    

            
        


}

