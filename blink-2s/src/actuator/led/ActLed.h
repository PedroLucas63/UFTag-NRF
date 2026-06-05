#pragma once
#include <stdint.h>

void actLedInit();
void actLedStart(uint16_t duration_ms);
void actLedTick();
void actLedOff();