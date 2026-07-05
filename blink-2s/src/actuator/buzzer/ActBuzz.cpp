#include "actuator/buzzer/ActBuzz.h"
#include "actuator/ActConfig.h"
#include "actuator/led/ActLed.h"
#include "state-machine/StateMachine.h"
#include <Arduino.h>

static volatile uint32_t buzzOffTimeMs = 0;

void actBuzzInit()
{
    pinMode(ACT_PIN_BUZZER, OUTPUT);
    noTone(ACT_PIN_BUZZER);
}

void actBuzzStart(uint16_t duration_ms)
{
    tone(ACT_PIN_BUZZER, ACT_BUZZ_FREQ_HZ);
    buzzOffTimeMs = millis() + duration_ms;
}

void actBuzzOff()
{
    noTone(ACT_PIN_BUZZER);
    buzzOffTimeMs = 0;
}

void runBuzzTick()
{
    static uint32_t buzzStartMs = 0;

    if (currentState == StateMachine::BUZZING)
    {
        if (buzzStartMs == 0)
        {
            buzzStartMs = millis();
            tone(ACT_PIN_BUZZER, ACT_BUZZ_FREQ_HZ);
            actLedSetRed(true); // LED Vermelho sincronizado
        }

        uint32_t elapsed = millis() - buzzStartMs;

        // 3 beeps de 0.75s (750ms) ligados e 0.25s (250ms) desligados (total: 3 segundos)
        if (elapsed < 3000)
        {
            uint32_t cycleTime = elapsed % 1000;
            if (cycleTime < 750)
            {
                // Beep ativo
                tone(ACT_PIN_BUZZER, ACT_BUZZ_FREQ_HZ);
                actLedSetRed(true);
            }
            else
            {
                // Silêncio
                noTone(ACT_PIN_BUZZER);
                actLedSetRed(false);
            }
        }
        else
        {
            // Fim do ciclo sonoro
            noTone(ACT_PIN_BUZZER);
            actLedSetRed(false);
            buzzStartMs = 0;
            Serial.println("[BUZZER] Ciclo de alerta sonoro concluído.");
            updateState(StateMachine::CONNECTED); // Retorna ao estado conectado
        }
    }
    else
    {
        buzzStartMs = 0;
    }
}