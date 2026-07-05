#include "BleGatt.h"
#include "../BleConfig.h"
#include "actuator/led/ActLed.h"
#include "actuator/buzzer/ActBuzz.h"
#include <Arduino.h>
#include "actuator/ActConfig.h"
#include "ble/store/KeyStore.h"
#include "ble/core/BleCore.h"
#include "nrf/NRFUtils.h"
#include "state-machine/StateMachine.h"

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

    bleResetLostModeTimer();

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
    case (CMD_BUZZ):
        updateState(StateMachine::BUZZING);
        break;
    case (CMD_LOST):
        bleSetLostModeState(true);
        break;
    default:
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

    bleResetLostModeTimer();

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

    bleResetLostModeTimer();

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

    bleResetLostModeTimer();

    if (len == 1)
    {
        bool active = (data[0] != 0);
        bleSetLostModeState(active);
    }
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
    characterGatt.setPermission(SECMODE_NO_ACCESS, SECMODE_ENC_NO_MITM); // Requer pareamento e conexão criptografada
    characterGatt.setWriteCallback(gattWrite);
    characterGatt.begin();

    // 3. Inicializa a Chave (Requer pareamento)
    characterKey.setPermission(SECMODE_NO_ACCESS, SECMODE_ENC_NO_MITM); // Requer pareamento e conexão criptografada
    characterKey.setWriteCallback(keyWrite);
    characterKey.begin();

    // 4. Atualização do nome da TAG
    characterName.setPermission(SECMODE_NO_ACCESS, SECMODE_ENC_NO_MITM); // Requer pareamento e conexão criptografada
    characterName.setWriteCallback(nameWrite);
    characterName.begin();

    // 5. Inicializa o Lost State
    characterLost.setPermission(SECMODE_ENC_NO_MITM, SECMODE_ENC_NO_MITM);
    characterLost.setWriteCallback(lostWrite);
    characterLost.begin();
    characterLost.write8(bleIsLostModeActive() ? 0x01 : 0x00);

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