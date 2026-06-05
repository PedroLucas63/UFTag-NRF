#include "ActLed.h"
#include "../ActConfig.h"
#include <Arduino.h>


// volatile: variável compartilhada entre loop() e callback BLE (ISR do stack)
static volatile uint32_t ledOffTimeMs = 0;

void actLedInit() {
    pinMode(ACT_PIN_LED_RED, OUTPUT);
    pinMode(ACT_PIN_LED_BLUE, OUTPUT);
    digitalWrite(ACT_PIN_LED_RED, ACT_LED_OFF);   
    digitalWrite(ACT_PIN_LED_BLUE, ACT_LED_OFF);
}

void actLedStart(uint16_t duration_ms) {
    digitalWrite(ACT_PIN_LED_RED, ACT_LED_ON);
    ledOffTimeMs = millis() + duration_ms;
}

void actLedTick() {
    if (ledOffTimeMs != 0 && millis() >= ledOffTimeMs) {
        digitalWrite(ACT_PIN_LED_RED, ACT_LED_OFF);
        ledOffTimeMs = 0; // desativa o timer
    }
}

void actLedOff() {
    digitalWrite(ACT_PIN_LED_RED, ACT_LED_OFF);
    ledOffTimeMs = 0; // cancela qualquer timer pendente
}