#pragma once

#include <Arduino.h>
#include "converter.h"
#include "charger.h"

// Battery thresholds (mV)
#define BATTERY_LOW_MV        7000
#define BATTERY_CRITICAL_MV   6800
// Power limits (W)
#define POWER_LIMIT_BASE_W   20
#define POWER_LIMIT_LOW_W    5

void power_management_update();