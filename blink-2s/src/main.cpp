#include <Arduino.h>
#include "ble/core/BleCore.h"
#include "ble/gatt/BleGatt.h"
#include "actuator/led/ActLed.h"
#include "actuator/buzzer/ActBuzz.h"
#include "ble/store/KeyStore.h"

void setup() {
    Serial.begin(115200);

    actLedInit();
    actBuzzInit();

    ksInit();

    bleInit();
    gattServiceInit();

    bleAdvertisingStart();

    Serial.println("UFTaf - Setup complete, waiting for BLE connections...");
}

void loop() {
    actBuzzTick();
    actLedTick();
}