// src/pwr/pwr_sleep.cpp
#include "power/PowerSleep.h"
#include "../actuator/buzzer/ActBuzz.h"
#include "../actuator/led/ActLed.h"
#include "../actuator/ActConfig.h"
#include <Arduino.h>
#include <bluefruit.h>   
#include <nrf_gpio.h>   

void powerEnterSleep() {
    Serial.println("[PWR] Entrando em System OFF...");
    Serial.flush();

    for (int i = 0; i < 2; i++) {
        digitalWrite(ACT_PIN_LED_RED, LOW);  
        delay(80);
        digitalWrite(ACT_PIN_LED_RED, HIGH); 
        delay(80);
    }

    // Garante que atuadores estão desligados
    actBuzzOff();
    actLedOff();

    nrf_gpio_cfg_sense_input(
        ACT_PIN_BUTTON,
        NRF_GPIO_PIN_PULLDOWN,
        NRF_GPIO_PIN_SENSE_HIGH
    );

    sd_power_system_off();

    while (true) { }
}
