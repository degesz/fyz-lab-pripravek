#pragma once

#include <Arduino.h>
#include <SimpleCLI.h>
#include "bms.h"
#include "converter.h"

extern bool stopLoop;
extern TPS55288 converter;

void setup_cli();

void handle_cli();