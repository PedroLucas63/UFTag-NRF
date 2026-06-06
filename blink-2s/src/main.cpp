#include <Arduino.h>
#include "ble/BleCore.h"
#include "ble/BleGatt.h"
#include "actuator/led/ActLed.h"
#include "actuator/buzzer/ActBuzz.h"

void setup() {
    Serial.begin(115200);

    actLedInit();
    actBuzzInit();

    bleInit();
    gattServiceInit();

    bleAdvertisingStart();

    Serial.println("UFTaf - Setup complete, waiting for BLE connections...");
}

void loop() {
    actBuzzTick();
    actLedTick();
}