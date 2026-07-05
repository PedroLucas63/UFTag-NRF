#include "BleCore.h"
#include "ble/BleConfig.h"
#include "ble/gatt/BleGatt.h"
#include "actuator/led/ActLed.h"
#include "actuator/buzzer/ActBuzz.h"
#include "actuator/ActConfig.h"
#include "ble/store/KeyStore.h"
#include "state-machine/StateMachine.h"
#include <bluefruit.h>
#include <services/BLEBas.h>
#include <Arduino.h>

static unsigned long lastCommunicationTime = 0;
static bool lostModeActive = false;
BLEBas blebas;

static uint8_t readBatteryLevel()
{
    int raw = analogRead(4);
    float voltage = raw * (3.6 / 1023.0) * 2.0;

    if (voltage >= 4.15) return 100;
    if (voltage <= 3.50) return 0;
    return (uint8_t)((voltage - 3.50) * 100.0 / 0.65);
}

static void setNameDevice()
{
    char savedName[17];
    if (ksGetName(savedName))
    {
        Bluefruit.setName(savedName);
    }
    else
    {
        uint8_t mac[6];
        Bluefruit.getAddr(mac);

        char name[16];
        snprintf(name, sizeof(name), DEVICE_NAME "-%02X%02X%02X", mac[0], mac[1], mac[2]);
        Bluefruit.setName(name);
    }
}

static void onConnect(uint16_t conn_hdl)
{
    BLEConnection *conn = Bluefruit.Connection(conn_hdl);
    if (conn == nullptr)
        return;
}

static void onDisconnect(uint16_t conn_hdl, uint8_t reason)
{
    (void)conn_hdl;
    (void)reason;
    bleAdvertisingSelectType();
}

static void onPairComplete(uint16_t conn_hdl, uint8_t auth_status)
{
    if (auth_status == 0)
    {
        BLEConnection *conn = Bluefruit.Connection(conn_hdl);
        if (conn != nullptr)
        {
            uint8_t ownAddres[6];
            memcpy(ownAddres, conn->getPeerAddr().addr, 6);
        }
    }
}

static void onSecured(uint16_t conn_hdl)
{
    BLEConnection *conn = Bluefruit.Connection(conn_hdl);

    if (conn == nullptr)
        return;

    if (ksHasKey() && !conn->bonded())
    {
        conn->disconnect();
        return;
    }

    updateState(StateMachine::CONNECTED);
    lastCommunicationTime = millis();
    bleSetLostModeState(false);
}

void bleInit()
{
    Bluefruit.begin();
    setNameDevice();
    Bluefruit.setTxPower(BLE_TX_POWER_DBM);

    Bluefruit.Security.setIOCaps(false, false, false);
    Bluefruit.Security.setMITM(false);
    Bluefruit.Security.setSecuredCallback(onSecured);
    Bluefruit.Security.setPairCompleteCallback(onPairComplete);

    Bluefruit.Periph.setConnectCallback(onConnect);
    Bluefruit.Periph.setDisconnectCallback(onDisconnect);

    // Inicializa o pino de leitura da bateria
    pinMode(4, INPUT);

    // Inicializa e expõe o Battery Service (GATT)
    blebas.begin();
    blebas.write(readBatteryLevel());

    lostModeActive = false;
    lastCommunicationTime = millis();

    Serial.println("[BLE] Stack initialized");
}

void bleAdvertisingSelectType()
{
    if (ksHasKey())
    {
        updateState(StateMachine::ADVERTISING);
    }
    else
    {
        updateState(StateMachine::PAIRING);
    }
}

// --- MODO PAREAMENTO (Aguardando Chave) ---
void bleAdvertisingStartPairing()
{
    Bluefruit.Advertising.clearData();
    Bluefruit.ScanResponse.clearData();

    Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
    Bluefruit.Advertising.addName();

    Bluefruit.Advertising.addService(gattGetService());

    // Também anuncia o Battery Service
    Bluefruit.Advertising.addService(blebas);

    Bluefruit.ScanResponse.addManufacturerData(MFR, sizeof(MFR));

    Bluefruit.Advertising.restartOnDisconnect(true);
    Bluefruit.Advertising.setInterval(
        BLE_ADV_PAIRING_INTERVAL, BLE_ADV_PAIRING_INTERVAL);
    Bluefruit.Advertising.start(0);
}

// --- MODO NORMAL ---
void bleAdvertisingStartNormal()
{
    Bluefruit.Advertising.clearData();
    Bluefruit.ScanResponse.clearData();

    Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);

    uint8_t pubKey[KEY_LEN];
    if (ksGet(pubKey))
    {
        // ADVERTISING: 26 bytes (23 + 3)
        uint8_t mfrMain[26];
        memcpy(&mfrMain[0], MFR, sizeof(MFR));
        mfrMain[2] = lostModeActive ? 0x01 : 0x00; // Lost flag

        memcpy(&mfrMain[3], pubKey, 23);

        Bluefruit.Advertising.addManufacturerData(mfrMain, sizeof(mfrMain));

        // SCAN RESPONSE: 12 bytes (2 + 1 + 9)
        // [UUID_RESPONSE (2b), Battery Level (1b), Remaining pubKey (9b)]
        uint8_t mfrScan[12];
        memcpy(&mfrScan[0], UUID_RESPONSE, sizeof(UUID_RESPONSE));
        mfrScan[2] = readBatteryLevel(); // Adiciona o nível de bateria
        memcpy(&mfrScan[3], pubKey + 23, 9);

        Bluefruit.ScanResponse.addData(
            BLE_GAP_AD_TYPE_SERVICE_DATA,
            mfrScan,
            sizeof(mfrScan));
    }

    // Adiciona o nome do dispositivo no Scan Response
    Bluefruit.ScanResponse.addName();

    // Estrategia fast/slow: 1s para economizar bateria
    Bluefruit.Advertising.restartOnDisconnect(true);
    Bluefruit.Advertising.setInterval(
        BLE_ADV_SLOW_INTERVAL, BLE_ADV_SLOW_INTERVAL); // Lento (1s) para economizar bateria
    Bluefruit.Advertising.start(0);
}

bool bleIsLostModeActive()
{
    return lostModeActive;
}

void bleSetLostModeState(bool active)
{
    lostModeActive = active;
    gattUpdateLostCharacteristic(active);

    if (!Bluefruit.connected())
    {
        bleAdvertisingStartNormal();
    }
}

void bleResetLostModeTimer()
{
    lastCommunicationTime = millis();
    if (lostModeActive)
    {
        bleSetLostModeState(false);
    }
}

void bleTick()
{
    // Atualiza periodicamente o serviço de bateria (a cada 60 segundos)
    static unsigned long lastBatteryUpdate = 0;
    if (millis() - lastBatteryUpdate > 60000)
    {
        lastBatteryUpdate = millis();
        blebas.write(readBatteryLevel());
    }

    if (Bluefruit.connected())
    {
        lastCommunicationTime = millis();
        if (lostModeActive)
        {
            bleSetLostModeState(false);
        }
    }
    else if (ksHasKey())
    {
        if (!lostModeActive && (millis() - lastCommunicationTime > BLE_COMPANION_TIMEOUT_MS))
        {
            bleSetLostModeState(true);
        }
    }
}
