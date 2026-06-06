#include "BleCore.h"
#include "BleConfig.h"
#include "BleGatt.h"
#include "actuator/led/ActLed.h"
#include "actuator/buzzer/ActBuzz.h"
#include <bluefruit.h>
#include <Arduino.h>


static void setNameDevice() {
    uint8_t mac[6];
    Bluefruit.getAddr(mac);

    char name[16];
    snprintf(name, sizeof(name), DEVICE_NAME "-%02X%02X%02X", mac[0], mac[1], mac[2]);
    Bluefruit.setName(name);
}

//========================================


void bleInit() {
    Bluefruit.begin();
    setNameDevice();

    Serial.println("[BLE] Stack initialized");
}

void bleAdvertisingStart() {

}
