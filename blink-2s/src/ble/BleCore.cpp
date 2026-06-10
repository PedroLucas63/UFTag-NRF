#include "BleCore.h"
#include "BleConfig.h"
#include "BleGatt.h"
#include "actuator/led/ActLed.h"
#include "actuator/buzzer/ActBuzz.h"
#include "actuator/ActConfig.h"
#include <bluefruit.h>
#include <Arduino.h>


static void setNameDevice() {
    uint8_t mac[6];
    Bluefruit.getAddr(mac);

    char name[16];
    snprintf(name, sizeof(name), DEVICE_NAME "-%02X%02X%02X", mac[0], mac[1], mac[2]);
    Bluefruit.setName(name);
}

static void onConnect(uint16_t conn_hdl) {
    (void)conn_hdl;
    Serial.println("[BLE] Dispositivo conectado");
    actLedStart(900);
    actBuzzerPip(ACT_PIN_BUZZER, 2, 400, 100);
}

static void onDisconnect(uint16_t conn_hdl, uint8_t reason) {
    (void)conn_hdl;
    actLedStart(500);
    actBuzzStart(500);
    bleAdvertisingStart();
}

static void onPairComplete(uint16_t conn_hdl, uint8_t auth_status) {
    (void)conn_hdl;
    if (auth_status == 0) {
        actLedStart(1400);
        actBuzzerPip(ACT_PIN_BUZZER, 3, 400, 100);
    } else {
        Serial.printf("[BLE] Falha no pareamento: 0x%02X\n", auth_status);
    }
}

void bleInit() {
    Bluefruit.begin();
    setNameDevice();
    Bluefruit.setTxPower(BLE_TX_POWER_DBM);

    // Segurança: "Just Works" — sem passkey (IO_CAPS_NONE)
    // IMPORTANTE: IO_CAPS_NONE é incompatível com setPairPasskeyCallback()
    Bluefruit.Security.setIOCaps(BLE_GAP_IO_CAPS_NONE);
    Bluefruit.Security.setMITM(false);
    Bluefruit.Security.setBond(true);  // bond persistente na flash interna
    Bluefruit.Security.setPairCompleteCallback(onPairComplete);

    Bluefruit.Periph.setConnectCallback(onConnect);
    Bluefruit.Periph.setDisconnectCallback(onDisconnect);

    Serial.println("[BLE] Stack initialized");
}

void bleAdvertisingStart() {
    Bluefruit.Advertising.clearData();
    Bluefruit.ScanResponse.clearData();

    // Payload do Advertisement (máx 31 bytes):
    //   Flags (3) + TxPower (3) + UUID 128-bit (18) = 24 bytes ✓
    Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
    Bluefruit.Advertising.addTxPower();
    Bluefruit.Advertising.addService(gattGetService()); // UUID 128-bit do serviço

    // Nome vai no Scan Response (evita ultrapassar 31 bytes no Advertisement)
    Bluefruit.ScanResponse.addName();
    Bluefruit.ScanResponse.addData();

    // Estratégia fast/slow: 100ms por 30s → 500ms para economizar bateria
    Bluefruit.Advertising.restartOnDisconnect(true);
    Bluefruit.Advertising.setInterval(BLE_ADV_FAST_INTERVAL, BLE_ADV_SLOW_INTERVAL);
    Bluefruit.Advertising.setFastTimeout(BLE_ADV_FAST_TIMEOUT);
    Bluefruit.Advertising.start(0); // 0 = sem timeout (advertise indefinidamente)

    Serial.println("[BLE] Advertising iniciado");
}
