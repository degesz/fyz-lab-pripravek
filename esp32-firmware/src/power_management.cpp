#include "power_management.h"

int power_limit = 20;
bool charging ;
bool low_bat;
bool critical_bat ;

void power_management_update()
{

    uint16_t vbus, vbat, vsys, ichg;
    read_charger(&vbus, &vbat, &vsys, &ichg);

    if (ichg > 0)
    {
        charging = true;
        power_limit = 20 + ((vbat / 1000) * (ichg / 1000));
    }
    else
    {
        charging = false;
        power_limit = 20;
    }

    low_bat = false;
    critical_bat = false;
    if (vbat <= 7000)
    {
        low_bat = true;
        power_limit = 5;
    }
    if (vbat <= 6800)
    {
        power_limit = 5;
        low_bat = true;
        critical_bat = true;
    }
    

    Serial.println("//////////////////   POWER MANAGEMENT/////////////\n");
    Serial.printf("     %s               --     Power limit: %d \n     Input voltage: %.1f     --     Charging current: %d \n", charging ? "Charging" : "Running on battery", power_limit, vbus / 1000.0, ichg);
    Serial.print(low_bat ? "BATTERY LOW\n\n" : "");
    Serial.print(critical_bat ? "BATTERY CRITICAL !! BATTERY CRITICAL !! BATTERY CRITICAL !! BATTERY CRITICAL !! \n\n" : "");
    Serial.println("//////////////////////////////////////////////////");


    setup_charger();
}