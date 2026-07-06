#include "ActLed.h"
#include "../ActConfig.h"
#include <Arduino.h>

// volatile: variável compartilhada entre loop() e callback BLE (ISR do stack)
static volatile uint32_t ledOffTimeMs = 0;

enum BlinkState
{
    BLINK_IDLE,
    BLINK_ON,
    BLINK_OFF
};

static BlinkState blinkState = BLINK_IDLE;
static uint8_t blinkPin = 0;
static uint8_t blinkRemaining = 0;
static uint16_t blinkOnMs = 0;
static uint16_t blinkOffMs = 0;
static uint32_t blinkNextMs = 0;

void actLedInit()
{
    pinMode(ACT_PIN_LED_RED, OUTPUT);
    pinMode(ACT_PIN_LED_BLUE, OUTPUT);
    digitalWrite(ACT_PIN_LED_RED, ACT_LED_OFF);
    digitalWrite(ACT_PIN_LED_BLUE, ACT_LED_OFF);

    // Sinaliza boot com 5 piscadas rápidas (assíncrono)
    actLedBlinkN(ACT_PIN_LED_RED, 5, 100, 100);
}

void actLedStart(uint16_t duration_ms)
{
    digitalWrite(ACT_PIN_LED_RED, ACT_LED_ON);
    ledOffTimeMs = millis() + duration_ms;
}

void actLedOff()
{
    digitalWrite(ACT_PIN_LED_RED, ACT_LED_OFF);
    ledOffTimeMs = 0; // cancela qualquer timer pendente
}

void actLedTick()
{
    if (ledOffTimeMs != 0 && millis() >= ledOffTimeMs)
    {
        actLedOff();
    }

    if (blinkState == BLINK_IDLE)
        return;
    if (millis() < blinkNextMs)
        return;

    if (blinkState == BLINK_ON)
    {
        digitalWrite(blinkPin, ACT_LED_OFF);
        blinkNextMs = millis() + blinkOffMs;
        blinkState = BLINK_OFF;
    }
    else
    {
        if (blinkRemaining == 0)
        {
            blinkState = BLINK_IDLE;
        }
        else
        {
            digitalWrite(blinkPin, ACT_LED_ON);
            blinkNextMs = millis() + blinkOnMs;
            blinkRemaining--;
            blinkState = BLINK_ON;
        }
    }
}

void actLedBlinkN(uint8_t pin, uint8_t times, uint16_t onMs, uint16_t offMs)
{
    blinkPin = pin;
    blinkRemaining = times - 1;
    blinkOnMs = onMs;
    blinkOffMs = offMs;

    digitalWrite(blinkPin, ACT_LED_ON);
    blinkNextMs = millis() + onMs;
    blinkState = BLINK_ON;
}