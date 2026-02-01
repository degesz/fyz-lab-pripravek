#include "power_management.h"
#include "device_state.h"
#include <Arduino.h>

static bool last_charging = false;
static bool last_low_bat = false;
static bool last_critical_bat = false;

void power_management_update()
{
    uint16_t vbus, vbat, vsys, ichg;
    read_charger(&vbus, &vbat, &vsys, &ichg);
    device_state_update_from_charger(vbus, vbat, vsys, ichg);

    const DeviceState* s = get_device_state();
    // Log only on state change to reduce Serial spam
    if (s->critical_bat != last_critical_bat)
    {
        last_critical_bat = s->critical_bat;
        if (s->critical_bat)
            Serial.println(F("POWER: BATTERY CRITICAL"));
    }
    if (s->low_bat != last_low_bat)
    {
        last_low_bat = s->low_bat;
        if (s->low_bat)
            Serial.println(F("POWER: Battery low"));
    }
    if (s->charging != last_charging)
    {
        last_charging = s->charging;
        Serial.printf("POWER: %s, limit %d W\n", s->charging ? "Charging" : "On battery", s->power_limit_w);
    }
}