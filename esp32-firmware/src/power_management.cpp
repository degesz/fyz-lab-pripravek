#include "power_management.h"
#include "device_state.h"
#include "charger.h"
#include <Arduino.h>

static bool last_charging = false;
static bool last_low_bat = false;
static bool last_critical_bat = false;

/** When VBUS is hot-plugged (absent -> present), re-init charger once. Avoid repeated
 *  reinit that could cause charger/relay clicking: require VBUS absent for several
 *  ticks before considering it "unplugged" again. */
static bool vbus_was_present = false;
static uint8_t vbus_stable_ticks = 0;
static uint8_t vbus_absent_ticks = 0;
static bool charger_reinited_this_plug = false;
#define VBUS_PRESENT_MV               4500
#define VBUS_ABSENT_MV                3800
#define VBUS_STABLE_TICKS_BEFORE_REINIT  2
#define VBUS_ABSENT_TICKS_BEFORE_UNPLUG  5

void power_management_update()
{
    uint16_t vbus, vbat, vsys, ichg;
    read_charger(&vbus, &vbat, &vsys, &ichg);
    device_state_update_from_charger(vbus, vbat, vsys, ichg);

    /* Re-init charger once when VBUS is hot-plugged. Avoid repeated reinit (causes clicking). */
    if (vbus >= VBUS_PRESENT_MV) {
        vbus_absent_ticks = 0;
        if (!vbus_was_present) {
            vbus_was_present = true;
            vbus_stable_ticks = 0;
            charger_reinited_this_plug = false;
        }
        vbus_stable_ticks++;
        if (vbus_stable_ticks == VBUS_STABLE_TICKS_BEFORE_REINIT && !charger_reinited_this_plug) {
            setup_charger();
            charger_reinited_this_plug = true;
        }
    } else if (vbus < VBUS_ABSENT_MV) {
        vbus_absent_ticks++;
        if (vbus_absent_ticks >= VBUS_ABSENT_TICKS_BEFORE_UNPLUG) {
            vbus_was_present = false;
            vbus_stable_ticks = 0;
            charger_reinited_this_plug = false;
        }
    } else {
        vbus_absent_ticks = 0;
    }

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