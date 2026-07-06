#pragma once 
#include <stdint.h>

void actBuzzInit();
void actBuzzStart(uint16_t duration_ms);
void actBuzzTick();
void actBuzzOff();
void actBuzzerPip(uint8_t pin, uint8_t times, uint16_t onMs, uint16_t offMs);
