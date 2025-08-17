#pragma once

#include <Arduino.h>
#include <SimpleCLI.h>
#include "converter.h"
#include "usb_pd.h"

extern bool stopLoop;
extern TPS55288 converter;
extern STUSB4500 usb;

void setup_cli();

void handle_cli();