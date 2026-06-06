#include "BleGatt.h"
#include "BleConfig.h"
#include "actuator/led/ActLed.h"
#include "actuator/buzzer/ActBuzz.h"
#include <Arduino.h>

// begin() é chamado em gattServiceInit(), após Bluefruit.begin()
static BLEService        serviceCmd(BLE_SVC_UUID);
static BLECharacteristic characterCmd(BLE_CHR_UUID, BLEWrite | BLEWriteWithoutResponse, 4);

static void gattWrite(
                    uint16_t           connHdl, 
                    BLECharacteristic* chr, 
                    uint8_t*           data, 
                    uint16_t           len
){
    (void)connHdl;
    (void)chr;

    if(len != 4) {
        Serial.println("[GATT] Invalid command");
        return;
    }

    uint8_t  action      = data[0];
    uint16_t durationMs = ((uint16_t)data[1] << 8) | data[2];

    Serial.printf("[GATT] Comando: 0x%02X  Duração: %dms\n", action, durationMs);

    switch (action){
    case (CMD_LED_BLINK):
        actLedStart(durationMs);
        break;
    case (CMD_BUZZ):
        actBuzzStart(durationMs);   
        break;
    case (CMD_BOTH):
        actLedStart(durationMs);
        actBuzzStart(durationMs);   
        break;
    case (CMD_STOP):
        actBuzzOff();
        actLedOff();
        break;
    default:
        Serial.printf("[GATT] Ação desconhecida: 0x%02X\n", action);
        break;
    }
}

void gattServiceInit() {
    serviceCmd.begin();
    characterCmd.setWriteCallback(gattWrite);
    characterCmd.begin();

    Serial.println("[GATT] Service initialized");
}