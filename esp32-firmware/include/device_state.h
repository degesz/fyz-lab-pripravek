#pragma once

#include <stdint.h>
#include <stdbool.h>

/**
 * Global device state: power limit, charging, battery flags, and latest
 * charger readings. Updated by power_management; read by display, CLI, etc.
 */
struct DeviceState {
    int power_limit_w;      /**< Current power limit in watts */
    bool charging;          /**< True when battery is charging (ichg > 0) */
    bool low_bat;           /**< True when battery voltage <= BATTERY_LOW_MV */
    bool critical_bat;      /**< True when battery voltage <= BATTERY_CRITICAL_MV */
    uint16_t vbus_mv;       /**< Input (VBUS) voltage in mV */
    uint16_t vbat_mv;       /**< Battery voltage in mV */
    uint16_t vsys_mv;       /**< System voltage in mV */
    uint16_t ichg_ma;       /**< Charge current in mA (0 when discharging) */
};

/** Returns read-only pointer to the current device state. */
const DeviceState* get_device_state(void);

/**
 * Updates device state from charger readings. Computes power_limit_w,
 * charging, low_bat, critical_bat and stores raw vbus/vbat/vsys/ichg.
 * Called by power_management after read_charger().
 */
void device_state_update_from_charger(uint16_t vbus_mv, uint16_t vbat_mv,
                                      uint16_t vsys_mv, uint16_t ichg_ma);
