#include "ActBuzz.h"
#include "../ActConfig.h"
#include "actuator/led/ActLed.h"
#include <Arduino.h>

static volatile uint32_t buzzOffTimeMs = 0;

enum PipState { PIP_IDLE, PIP_ON, PIP_OFF};

static PipState    pipState     = PIP_IDLE;
static uint8_t     pipPin       = 0;
static uint8_t     pipRemaining = 0;
static uint16_t    pipOnMs      = 0;
static uint16_t    pipOffMs     = 0;
static uint32_t    pipNextMs    = 0;

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

    if(pipState == PIP_IDLE) return;
    // if(millis() > pipNextMs) return;

    if(pipState == PIP_ON){
        noTone(pipPin);
        pipState = PIP_OFF;
        pipNextMs = millis() + pipOffMs;
    } else{
        pipRemaining--;
        if(pipRemaining == 0){
            pipState = PIP_IDLE;
        }else{
            tone(pipPin, ACT_BUZZ_FREQ_HZ);
            pipNextMs = millis() + pipOnMs;
            pipState = PIP_ON;
        }
    }
}

void actBuzzerPip(uint8_t pin, uint8_t times, uint16_t onMs, uint16_t offMs){
    pipPin = pin;
    pipRemaining = times;
    pipOnMs = onMs;
    pipOffMs = offMs;

    tone(pipPin, ACT_BUZZ_FREQ_HZ);
    pipNextMs = millis() + onMs;
    pipState = PIP_ON;
}

void actBuzzOff() {
    noTone(ACT_PIN_BUZZER);
    buzzOffTimeMs = 0;
}