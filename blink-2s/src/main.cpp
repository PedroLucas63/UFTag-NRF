#include <Arduino.h>
#include "actuator/led/ActLed.h"
#include "actuator/button/ActButton.h"
#include "state-machine/StateMachine.h"

void setup()
{
    actLedInit();
    btnInit();
}

void loop()
{
    run();
}