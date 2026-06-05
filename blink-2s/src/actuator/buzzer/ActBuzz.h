#pragma once 
#include <stdint.h>

void actBuzzInit();
void actBuzzStart(uint16_t duration_ms);
void actBuzzTick();
void actBuzzOff();
void actStopAll();