#include "BleCore.h"
#include "../BleConfig.h"
#include "../gatt/BleGatt.h"
#include "actuator/led/ActLed.h"
#include "actuator/buzzer/ActBuzz.h"
#include "actuator/ActConfig.h"
#include "ble/store/KeyStore.h"
#include <bluefruit.h>
#include <Arduino.h>

static void setNameDevice()
{
    uint8_t mac[6];
    Bluefruit.getAddr(mac);

    char name[16];
    snprintf(name, sizeof(name), DEVICE_NAME "-%02X%02X%02X", mac[0], mac[1], mac[2]);
    Bluefruit.setName(name);
}

static void onConnect(uint16_t conn_hdl)
{
    BLEConnection *conn = Bluefruit.Connection(conn_hdl);
    if (conn == nullptr)
        return;

    // Aceita pareamento apenas do dono, rejeitando conexões de outros dispositivos
    // TODO: E se o dono trocar de celular?
    if (ksHasKey())
    {
        if (!conn->bonded())
        {
            Serial.println("[BLE] Intruso detectado! Desconectando...");
            conn->disconnect();
            return;
        }

        Serial.println("[BLE] Dispositivo pareado conectado.");
    }
    else
    {
        Serial.println("[BLE] Primeira conexão. Iniciando pareamento com o novo dono.");
    }
}

static void onDisconnect(uint16_t conn_hdl, uint8_t reason)
{
    (void)conn_hdl;
    (void)reason;
    actLedStart(500);
    // actBuzzStart(500);
    bleAdvertisingStart();
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
            conn->getPeerAddr().addr;
        }

        actLedBlinkN(ACT_PIN_LED_RED, 3, 400, 100);
        // actBuzzerPip(ACT_PIN_BUZZER, 3, 400, 100);
    }
    else
    {
        Serial.printf("[BLE] Falha no pareamento: 0x%02X\n", auth_status);
    }
}

static void onSecured(uint16_t conn_hdl)
{
    BLEConnection *conn = Bluefruit.Connection(conn_hdl);

    if (conn == nullptr)
        return;
    if (conn->bonded())
    {
        actLedBlinkN(ACT_PIN_LED_RED, 3, 1000, 100);
        // actBuzzerPip(ACT_PIN_BUZZER, 2, 400, 100);
    }
    else
    {
        actLedBlinkN(ACT_PIN_LED_RED, 3, 500, 100);
    }
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

    Serial.println("[BLE] Stack initialized");
}

void bleAdvertisingStart()
{
    Bluefruit.Advertising.clearData();
    Bluefruit.ScanResponse.clearData();

    // Flags (3)
    Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
    // Bluefruit.Advertising.addTxPower();
    Bluefruit.Advertising.addService(gattGetService());

    uint8_t pubKey[KEY_LEN];
    if (ksGet(pubKey))
    {
        uint8_t adv[26];
        memcpy(&adv[0], pubKey, 26);
        Bluefruit.Advertising.addData(1, adv, sizeof(adv));
    }

    // Nome do Dispositivo no Scan Response (~14 bytes)
    Bluefruit.ScanResponse.addName();

    if (ksGet(pubKey))
    {
        uint8_t mfr[8];
        mfr[0] = BLE_COMPANY_ID_LO;
        mfr[1] = BLE_COMPANY_ID_HI;
        memcpy(&mfr[2], pubKey + 26, 6);

        Bluefruit.ScanResponse.addManufacturerData(mfr, sizeof(mfr));
    }

    // Estratégia fast/slow: 100ms por 30s → 500ms para economizar bateria
    Bluefruit.Advertising.restartOnDisconnect(true);
    Bluefruit.Advertising.setInterval(BLE_ADV_FAST_INTERVAL, BLE_ADV_SLOW_INTERVAL);
    Bluefruit.Advertising.setFastTimeout(BLE_ADV_FAST_TIMEOUT);
    Bluefruit.Advertising.start(0); // 0 = sem timeout

    Serial.println("[BLE] Advertising iniciado");
}

// --- MODO PAREAMENTO (Aguardando Chave) ---
void bleAdvertisingStartPairing()
{
    Bluefruit.Advertising.clearData();
    Bluefruit.ScanResponse.clearData();

    Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
    Bluefruit.Advertising.addName();

    // Adiciona o serviço GATT que o celular vai usar para mandar a chave
    Bluefruit.Advertising.addService(gattGetService());

    Bluefruit.Advertising.restartOnDisconnect(true);
    Bluefruit.Advertising.setInterval(160, 160); // Rápido (100ms) para ser achado logo
    Bluefruit.Advertising.start(0);

    Serial.println("[BLE] Advertising de PAREAMENTO iniciado");
}

// --- MODO NORMAL (Fictício "Estou Aqui") ---
void bleAdvertisingStartNormal()
{
    Bluefruit.Advertising.clearData();
    Bluefruit.ScanResponse.clearData();

    Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
    Bluefruit.Advertising.addName();

    // Uma flag fictícia simples (ex: Manufacturer Data com 1 byte '0x01' significando "Estou Aqui")
    uint8_t payload[] = {0x01};
    Bluefruit.Advertising.addManufacturerData(payload, sizeof(payload));

    Bluefruit.Advertising.restartOnDisconnect(true);
    Bluefruit.Advertising.setInterval(800, 800); // Lento (500ms) para economizar bateria
    Bluefruit.Advertising.start(0);

    Serial.println("[BLE] Advertising NORMAL iniciado (Estou Aqui)");
}