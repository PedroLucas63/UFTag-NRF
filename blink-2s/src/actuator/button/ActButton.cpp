#include "actuator/button/ActButton.h"
#include "actuator/led/ActLed.h"
#include "actuator/ActConfig.h"
#include "state-machine/StateMachine.h"
#include <Arduino.h>

static uint32_t pressedAt = 0;
static bool btnWasDown = false;

void btnInit()
{
    pinMode(ACT_PIN_BUTTON, INPUT_PULLUP);
}

void btnTick()
{
    bool isDown = (digitalRead(ACT_PIN_BUTTON) == LOW);

    if (currentState == StateMachine::DISABLED)
    {
        if (isDown)
        {
            if (!btnWasDown)
            {
                pressedAt = millis();
                btnWasDown = true;
            }
            else
            {
                uint32_t holdDuration = millis() - pressedAt;

                if ((holdDuration % 200) < 100)
                {
                    actLedSetRed(true);
                }
                else
                {
                    actLedSetRed(false);
                }

                if (holdDuration >= 3000)
                {
                    actLedSetRed(false);
                    btnWasDown = false;
                    updateState(StateMachine::INIT);
                }
            }
        }
        else
        {
            if (btnWasDown)
            {
                actLedSetRed(false);
                btnWasDown = false;
            }
        }
    }
}
