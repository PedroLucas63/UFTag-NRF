#include "BleCore.h"
#include "../BleConfig.h"
#include "../gatt/BleGatt.h"
#include "actuator/led/ActLed.h"
#include "actuator/buzzer/ActBuzz.h"
#include "actuator/ActConfig.h"
#include "ble/store/KeyStore.h"
#include <bluefruit.h>
#include <Arduino.h>

static unsigned long lastCommunicationTime = 0;
static bool lostModeActive = false;

static void setNameDevice()
{
    char savedName[17];
    if (ksGetName(savedName))
    {
        Serial.printf("[BLE] Nome carregado da flash: %s\n", savedName);
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

    if (ksHasKey())
    {
        Serial.println("[BLE] Alguém conectou. Aguardando criptografia...");
    }
    else
    {
        Serial.println("[BLE] Primeira conexão. Iniciando pareamento com o novo dono.");
    }
}

static void onDisconnect(uint16_t conn_hdl, uint8_t reason)
{
    Serial.printf("[BLE] Desconectado (reason: 0x%02X)\n", reason);
    (void)conn_hdl;
    (void)reason;
    actLedStart(500);
    // actBuzzStart(500);
    bleAdvertisingSwitchType();


    // Inicia contagem do tempo a partir da desconexão
    lastCommunicationTime = millis();
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

    if (ksHasKey())
    {
        if (!conn->bonded())
        {
            Serial.println("[BLE] Intruso detectado! Desconectando...");
            conn->disconnect();
            return;
        }

        Serial.println("[BLE] Dono reconhecido! Conexão segura estabelecida.");
        actLedBlinkN(ACT_PIN_LED_RED, 3, 750, 100);
        // actBuzzerPip(ACT_PIN_BUZZER, 2, 400, 100);
    }
    else
    {
        Serial.println("[BLE] Conexão segura iniciada (Aguardando chave do novo dono...)");
        actLedBlinkN(ACT_PIN_LED_RED, 3, 500, 100);
    }

    // Conexão segura do dono reseta o tempo de comunicação e sai do modo perdido
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

    // Inicializa a variável com o valor persistido na flash
    lostModeActive = ksGetLostState();

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

void bleAdvertisingSwitchType()
{
    if (ksHasKey())
    {
        Serial.println("[DEBUG] Chave de acesso encontrada no KeyStore.");
        bleAdvertisingStartNormal();
    }
    else
    {
        Serial.println("[DEBUG] Nenhuma chave de acesso encontrada no KeyStore.");
        bleAdvertisingStartPairing();
    }
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

    // Adiciona o Company ID para identificação na aplicação
    Bluefruit.ScanResponse.addManufacturerData(MFR, sizeof(MFR));

    // Estratégia: 100ms para ser achado logo, sem timeout (fica até o dono parear)
    Bluefruit.Advertising.restartOnDisconnect(true);
    Bluefruit.Advertising.setInterval(
        BLE_ADV_PAIRING_INTERVAL, BLE_ADV_PAIRING_INTERVAL); // Rápido (100ms) para ser achado logo
    Bluefruit.Advertising.start(0);

    Serial.println("[BLE] Advertising de PAREAMENTO iniciado");
}

// --- MODO NORMAL (Fictício "Estou Aqui") ---
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

        // SCAN RESPONSE: 11 bytes (2 + 9)
        uint8_t mfrScan[11];
        memcpy(&mfrScan[0], UUID_RESPONSE, sizeof(UUID_RESPONSE));
        memcpy(&mfrScan[2], pubKey + 23, 9);

        Bluefruit.ScanResponse.addData(
            BLE_GAP_AD_TYPE_SERVICE_DATA,
            mfrScan,
            sizeof(mfrScan));
    }

    // Adicionar 16 bytes no Scan Response
    Bluefruit.ScanResponse.addName();

    // Estratégia fast/slow: 500ms para economizar bateria
    Bluefruit.Advertising.restartOnDisconnect(true);
    Bluefruit.Advertising.setInterval(
        BLE_ADV_SLOW_INTERVAL, BLE_ADV_SLOW_INTERVAL); // Lento (500ms) para economizar bateria
    Bluefruit.Advertising.start(0);

    Serial.println("[BLE] Advertising NORMAL iniciado");
}


bool bleIsLostModeActive()
{
    return lostModeActive;
}

void bleSetLostModeState(bool active)
{
    if (lostModeActive != active)
    {
        lostModeActive = active;
        ksSaveLostState(active);
        
        // Atualiza a característica GATT
        gattUpdateLostCharacteristic(active);

        Serial.printf("[BLE] Modo Companio alterado para: %s\n", active ? "ATIVO" : "INATIVO");

        // Se o dispositivo estiver desconectado, reiniciamos o advertising para atualizar a flag
        if (!Bluefruit.connected())
        {
            Serial.println("[BLE] Reiniciando advertising com novo estado.");
            bleAdvertisingStartNormal();
        }
    }
}

void bleTick()
{
    // Apenas monitora timeout se tiver um dono emparelhado e estiver desconectado
    if (ksHasKey() && !Bluefruit.connected())
    {
        if (!lostModeActive && (millis() - lastCommunicationTime > BLE_COMPANION_TIMEOUT_MS))
        {
            Serial.println("[BLE] Timeout de comunicação excedido sem presença do dono!");
            bleSetLostModeState(true);
        }
    }
}
