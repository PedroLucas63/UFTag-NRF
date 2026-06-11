#include "BleGatt.h"
#include "../BleConfig.h"
#include "actuator/led/ActLed.h"
#include "actuator/buzzer/ActBuzz.h"
#include <Arduino.h>
#include "actuator/ActConfig.h"
#include "ble/store/KeyStore.h"
#include "ble/core/BleCore.h"

// begin() é chamado em gattServiceInit(), após Bluefruit.begin()
static BLEService        serviceGatt(BLE_SVC_UUID);
static BLECharacteristic characterGatt(BLE_CHR_UUID, BLEWrite | BLEWriteWithoutResponse, 4);
static BLECharacteristic characterKey(BLE_CHR_SETKEY_UUID, BLEWrite, KEY_LEN);

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
    
    case (CMD_ALERT):
        actBuzzerPip(ACT_PIN_BUZZER, 10, 100, 900);
        actLedBlinkN(ACT_PIN_LED_RED, 20, 100, 400);
        break;
    default:
        Serial.printf("[GATT] Ação desconhecida: 0x%02X\n", action);
        break;
    }
}

static void keyWrite(
                    uint16_t           connHdl, 
                    BLECharacteristic* chr, 
                    uint8_t*           data, 
                    uint16_t           len
){
    (void) connHdl;
    (void) chr;

    if(len != KEY_LEN) return;

    if(ksSave(data)){
        Bluefruit.Advertising.stop();
        bleAdvertisingStart();
    }
}


void gattServiceInit() {
    serviceGatt.begin();
    characterGatt.setWriteCallback(gattWrite);
    characterGatt.begin();

    characterKey.setPermission(SECMODE_ENC_WITH_MITM, SECMODE_NO_ACCESS); // Required paring and encripted conection
    characterKey.setWriteCallback(keyWrite);
    characterKey.begin();

    Serial.println("[GATT] Service initialized");
}


BLEService& gattGetService(){
    return serviceGatt;
}