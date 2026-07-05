#pragma once
#include <stdint.h>

void actLedInit();
void actLedStart(uint16_t duration_ms);
void actLedOff();
void runLedTick();

void actLedSetRed(bool state);
void actLedSetBlue(bool state);
void actLedPlayInitSinalization();
void actLedPlaySleepSinalization();
void actLedPlayErrorSinalization();