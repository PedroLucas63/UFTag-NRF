#pragma once
#include <stdint.h>

void actLedInit();
void actLedStart(uint16_t duration_ms);
void actLedTick();
void actLedBlinkN(uint8_t pin, uint8_t times, uint16_t onMs, uint16_t offMs);
void actLedOff();