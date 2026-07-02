#include "BleGatt.h"
#include "../BleConfig.h"
#include "actuator/led/ActLed.h"
#include "actuator/buzzer/ActBuzz.h"
#include <Arduino.h>
#include "actuator/ActConfig.h"
#include "ble/store/KeyStore.h"
#include "ble/core/BleCore.h"
#include "nrf/NRFUtils.h"

// begin() é chamado em gattServiceInit(), após Bluefruit.begin()
static BLEService serviceGatt(BLE_SVC_UUID);

static BLECharacteristic characterId(BLE_CHR_ID_UUID, BLERead, NRF_DEVICE_ID_LEN);
static BLECharacteristic characterGatt(BLE_CHR_UUID, BLEWrite | BLEWriteWithoutResponse, 4);
static BLECharacteristic characterKey(BLE_CHR_SETKEY_UUID, BLEWrite, KEY_LEN);
static BLECharacteristic characterName(BLE_CHR_NAME_UUID, BLEWrite, NAME_LEN);
static BLECharacteristic characterLost(BLE_CHR_LOST_UUID, BLERead | BLEWrite, 1);

static void gattWrite(
    uint16_t connHdl,
    BLECharacteristic *chr,
    uint8_t *data,
    uint16_t len)
{
    (void)connHdl;
    (void)chr;

    if (len != 4)
    {
        Serial.println("[GATT] Invalid command");
        return;
    }

    uint8_t action = data[0];
    uint16_t durationMs = ((uint16_t)data[1] << 8) | data[2];

    Serial.printf("[GATT] Comando: 0x%02X  Duração: %dms\n", action, durationMs);

    switch (action)
    {
    case (CMD_LED_BLINK):
        Serial.printf("[GATT] Ação: LED Blink  Duração: %dms\n", durationMs);
        actLedStart(durationMs);
        break;
    case (CMD_BUZZ):
        Serial.printf("[GATT] Ação: Buzzer  Duração: %dms\n", durationMs);
        // actBuzzStart(durationMs);
        break;
    case (CMD_BOTH):
        Serial.printf("[GATT] Ação: LED e Buzzer  Duração: %dms\n", durationMs);
        actLedStart(durationMs);
        // actBuzzStart(durationMs);
        break;
    case (CMD_STOP):
        Serial.println("[GATT] Ação: Parar tudo");
        // actBuzzOff();
        actLedOff();
        break;

    case (CMD_ALERT):
        Serial.printf("[GATT] Ação: ALERTA MÁXIMO!  Duração: %dms\n", durationMs);
        // actBuzzerPip(ACT_PIN_BUZZER, 10, 100, 900);
        actLedBlinkN(ACT_PIN_LED_RED, 20, 100, 400);
        break;
    default:
        Serial.printf("[GATT] Ação desconhecida: 0x%02X\n", action);
        break;
    }
}

static void keyWrite(
    uint16_t connHdl,
    BLECharacteristic *chr,
    uint8_t *data,
    uint16_t len)
{
    (void)chr;

    if (len != KEY_LEN)
    {
        Serial.println("[GATT] Erro: Tamanho de chave incorreto!");
        return;
    }

    if (ksSave(data))
    {
        Serial.println("[GATT] Nova chave salva com sucesso!");
        for (int i = 0; i < KEY_LEN; i++)
        {
            Serial.printf("%02X", data[i]);
        }
        Serial.println();

        BLEConnection *conn = Bluefruit.Connection(connHdl);
        if (conn != nullptr)
        {
            conn->disconnect();
        }
    }
}


static void nameWrite(
    uint16_t connHdl,
    BLECharacteristic *chr,
    uint8_t *data,
    uint16_t len)
{
    (void)chr;

    if (len == 0 || len > NAME_LEN)
    {
        Serial.println("[GATT] Erro: Tamanho do nome incorreto!");
        return;
    }

    if (updateName(data, len))
    {
        Serial.println("[GATT] Novo nome salvo com sucesso!");
        for (int i = 0; i < len; i++)
        {
            Serial.printf("%02X", data[i]);
        }
        Serial.println();

        BLEConnection *conn = Bluefruit.Connection(connHdl);
        if (conn != nullptr)
        {
            conn->disconnect();
        }
    }
}

static void lostWrite(
    uint16_t connHdl,
    BLECharacteristic *chr,
    uint8_t *data,
    uint16_t len)
{
    (void)connHdl;
    (void)chr;

    if (len != 1)
    {
        Serial.println("[GATT] Erro: Tamanho inválido para lost flag!");
        return;
    }

    bool newStatus = (data[0] != 0);
    bleSetLostModeState(newStatus);
}


void gattServiceInit()
{
    serviceGatt.begin();

    // 1. Inicializa o ID
    characterId.setPermission(SECMODE_ENC_NO_MITM, SECMODE_NO_ACCESS);
    characterId.begin();

    char deviceId[NRF_DEVICE_ID_LEN + 1];
    getDeviceId(deviceId);
    characterId.write(deviceId, NRF_DEVICE_ID_LEN);

    // 2. Inicializa os Comandos
    characterGatt.setPermission(SECMODE_NO_ACCESS, SECMODE_ENC_NO_MITM); // Required paring and encripted conection
    characterGatt.setWriteCallback(gattWrite);
    characterGatt.begin();

    // 3. Inicializa a Chave (Requer pareamento)
    characterKey.setPermission(SECMODE_NO_ACCESS, SECMODE_ENC_NO_MITM); // Required paring and encripted conection
    characterKey.setWriteCallback(keyWrite);
    characterKey.begin();

    // 3. Atualizacao do nome da TAG
    characterName.setPermission(SECMODE_NO_ACCESS, SECMODE_ENC_NO_MITM); // Required paring and encripted conection
    characterName.setWriteCallback(nameWrite);
    characterName.begin();


    // 4. Modo Companio/Lost
    characterLost.setPermission(SECMODE_ENC_NO_MITM, SECMODE_ENC_NO_MITM);
    characterLost.setWriteCallback(lostWrite);
    characterLost.begin();
    characterLost.write8(ksGetLostState() ? 0x01 : 0x00);

    Serial.println("[GATT] Service initialized");
}

BLEService &gattGetService()
{
    return serviceGatt;
}

void gattUpdateLostCharacteristic(bool active)
{
    characterLost.write8(active ? 0x01 : 0x00);
}
