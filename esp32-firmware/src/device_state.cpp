#include "device_state.h"
#include "power_management.h"
#include <Arduino.h>

static DeviceState state = {
    .power_limit_w = POWER_LIMIT_BASE_W,
    .charging = false,
    .low_bat = false,
    .critical_bat = false,
    .vbus_mv = 0,
    .vbat_mv = 0,
    .vsys_mv = 0,
    .ichg_ma = 0,
};

const DeviceState* get_device_state(void)
{
    return &state;
}

void device_state_update_from_charger(uint16_t vbus_mv, uint16_t vbat_mv,
                                      uint16_t vsys_mv, uint16_t ichg_ma)
{
    state.vbus_mv = vbus_mv;
    state.vbat_mv = vbat_mv;
    state.vsys_mv = vsys_mv;
    state.ichg_ma = ichg_ma;

    if (ichg_ma > 0)
    {
        state.charging = true;
        state.power_limit_w = POWER_LIMIT_BASE_W + ((vbat_mv / 1000) * (ichg_ma / 1000));
    }
    else
    {
        state.charging = false;
        state.power_limit_w = POWER_LIMIT_BASE_W;
    }

    state.low_bat = false;
    state.critical_bat = false;
    if (vbat_mv <= BATTERY_LOW_MV)
    {
        state.low_bat = true;
        state.power_limit_w = POWER_LIMIT_LOW_W;
    }
    if (vbat_mv <= BATTERY_CRITICAL_MV)
    {
        state.power_limit_w = POWER_LIMIT_LOW_W;
        state.low_bat = true;
        state.critical_bat = true;
    }
}
