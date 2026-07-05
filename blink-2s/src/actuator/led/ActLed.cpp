#include "actuator/led/ActLed.h"
#include "actuator/ActConfig.h"
#include "state-machine/StateMachine.h"
#include "ble/core/BleCore.h"
#include <Arduino.h>

static volatile uint32_t ledOffTimeMs = 0;

void actLedInit()
{
    pinMode(ACT_PIN_LED_RED, OUTPUT);
    pinMode(ACT_PIN_LED_BLUE, OUTPUT);
    digitalWrite(ACT_PIN_LED_RED, ACT_LED_OFF);
    digitalWrite(ACT_PIN_LED_BLUE, ACT_LED_OFF);
}

void actLedStart(uint16_t duration_ms)
{
    digitalWrite(ACT_PIN_LED_RED, ACT_LED_ON);
    ledOffTimeMs = millis() + duration_ms;
}

void actLedOff()
{
    digitalWrite(ACT_PIN_LED_RED, ACT_LED_OFF);
    digitalWrite(ACT_PIN_LED_BLUE, ACT_LED_OFF);
    ledOffTimeMs = 0;
}

void runLedTick()
{
    static uint32_t lastLedToggle = 0;
    static bool ledState = false;

    // Se estiver no modo BUZZING, a sincronização é feita pelo runBuzzTick, então não interferimos aqui
    if (currentState == StateMachine::BUZZING)
    {
        return;
    }

    switch (currentState)
    {
    case StateMachine::DISABLED:
        digitalWrite(ACT_PIN_LED_RED, ACT_LED_OFF);
        digitalWrite(ACT_PIN_LED_BLUE, ACT_LED_OFF);
        break;

    case StateMachine::INIT:
        // Piscando rápido durante a inicialização (já tratado de forma bloqueante no init_device para garantir feedback imediato)
        break;

    case StateMachine::PAIRING:
        // Ticks contínuos rápidos (ex: 150ms ligado, 150ms desligado no LED Vermelho)
        digitalWrite(ACT_PIN_LED_BLUE, ACT_LED_OFF);
        if (millis() - lastLedToggle >= 150)
        {
            ledState = !ledState;
            digitalWrite(ACT_PIN_LED_RED, ledState ? ACT_LED_ON : ACT_LED_OFF);
            lastLedToggle = millis();
        }
        break;

    case StateMachine::ADVERTISING:
        // Sem ticks (LED desligado para economizar bateria) ou piscando bem lento se em modo perdido
        digitalWrite(ACT_PIN_LED_BLUE, ACT_LED_OFF);
        if (bleIsLostModeActive())
        {
            // Se estiver em modo perdido, pisca o LED vermelho a cada 2 segundos rapidamente
            uint32_t cycle = millis() % 2000;
            if (cycle < 100)
            {
                digitalWrite(ACT_PIN_LED_RED, ACT_LED_ON);
            }
            else
            {
                digitalWrite(ACT_PIN_LED_RED, ACT_LED_OFF);
            }
        }
        else
        {
            digitalWrite(ACT_PIN_LED_RED, ACT_LED_OFF);
        }
        break;

    case StateMachine::CONNECTED:
    case StateMachine::CONFIGURING:
        // Led azul aceso contínuo para indicar conexão com dono
        digitalWrite(ACT_PIN_LED_RED, ACT_LED_OFF);
        digitalWrite(ACT_PIN_LED_BLUE, ACT_LED_ON);
        break;

    case StateMachine::ERROR:
        // Piscada lenta de erro é tratada pela actLedPlayErrorSinalization() de forma bloqueante
        break;

    case StateMachine::BUZZING:
        break;
    }
}

void actLedSetRed(bool state)
{
    digitalWrite(ACT_PIN_LED_RED, state ? ACT_LED_ON : ACT_LED_OFF);
}

void actLedSetBlue(bool state)
{
    digitalWrite(ACT_PIN_LED_BLUE, state ? ACT_LED_ON : ACT_LED_OFF);
}

void actLedPlayInitSinalization()
{
    for (int i = 0; i < 5; i++)
    {
        digitalWrite(ACT_PIN_LED_RED, ACT_LED_ON);
        delay(100);
        digitalWrite(ACT_PIN_LED_RED, ACT_LED_OFF);
        delay(100);
    }
}

void actLedPlaySleepSinalization()
{
    for (int i = 0; i < 2; i++)
    {
        digitalWrite(ACT_PIN_LED_RED, ACT_LED_ON);
        delay(80);
        digitalWrite(ACT_PIN_LED_RED, ACT_LED_OFF);
        delay(80);
    }
}

void actLedPlayErrorSinalization()
{
    for (int i = 0; i < 3; i++)
    {
        digitalWrite(ACT_PIN_LED_RED, ACT_LED_ON);
        delay(500);
        digitalWrite(ACT_PIN_LED_RED, ACT_LED_OFF);
        delay(500);
    }
}