#include "ActButton.h"
#include "actuator/led/ActLed.h"
#include <Arduino.h>
#include "../ActConfig.h"
#include "power/PowerSleep.h"

static uint32_t pressedAt = 0;
static bool btnWasDown = false;

void btnInit()
{
    pinMode(ACT_PIN_BUTTON, INPUT_PULLUP);
}

void btnTick()
{
    bool down = (digitalRead(ACT_PIN_BUTTON) == HIGH);

    if (down && !btnWasDown)
    {
        Serial.println("[BTN] Pressionado");
        pressedAt = millis();
        btnWasDown = true;
    }

    if (!down && btnWasDown)
    {
        Serial.println("[BTN] Solto");
        uint32_t held = millis() - pressedAt;
        btnWasDown = false;

        if (held >= HOLD_TO_SLEEP_MS)
        {
            Serial.println("[BTN] COLOCANDO PARA DORMIR!");
        }
    }
}
