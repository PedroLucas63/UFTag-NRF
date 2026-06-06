#include "ActBuzz.h"
#include "../ActConfig.h"
#include "actuator/led/ActLed.h"
#include <Arduino.h>

static volatile uint32_t buzzOffTimeMs = 0;

void actBuzzInit() {
    pinMode(ACT_PIN_BUZZER, OUTPUT);
    noTone(ACT_PIN_BUZZER); 
}

void actBuzzStart(uint16_t duration_ms) {
    tone(ACT_PIN_BUZZER, ACT_BUZZ_FREQ_HZ);
    buzzOffTimeMs = millis() + duration_ms;
}

void actBuzzTick() {
    if (buzzOffTimeMs != 0 && millis() >= buzzOffTimeMs) {
        noTone(ACT_PIN_BUZZER);
        buzzOffTimeMs = 0;
    }
}

void actBuzzOff() {
    noTone(ACT_PIN_BUZZER);
    buzzOffTimeMs = 0;
}