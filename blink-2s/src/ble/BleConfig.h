#pragma once

#include <stdint.h>

#define DEVICE_NAME "UFTag"

#define BLE_SVC_UUID 0xFFF0
#define BLE_CHR_UUID 0xFFF1
#define BLE_CHR_SETKEY_UUID 0xFFF2
#define BLE_CHR_ID_UUID 0xFFF3
#define BLE_CHR_NAME_UUID 0xFFF4

// ─── Tamanho da chave pública (Curve25519) ────────────────────────────────────
#define BLE_PUB_KEY_LEN 32

// ─── ID da Empresa ────────────────────────────────────
#define BLE_COMPANY_ID_LO 0xFF
#define BLE_COMPANY_ID_HI 0xFF
#define BLE_RESPONSE_COMPANY_ID_HI 0xAB
constexpr uint8_t MFR[2] = {BLE_COMPANY_ID_LO, BLE_COMPANY_ID_HI};
constexpr uint8_t UUID_RESPONSE[2] = {BLE_COMPANY_ID_LO, BLE_RESPONSE_COMPANY_ID_HI};

// ─── Advertising ──────────────────────────────────────────────────────────────
#define BLE_ADV_PAIRING_INTERVAL 160 // 100ms  (unidade: 0.625ms)
#define BLE_ADV_FAST_INTERVAL 160    // 100ms  (unidade: 0.625ms)
#define BLE_ADV_SLOW_INTERVAL 800    // 500ms
#define BLE_ADV_FAST_TIMEOUT 30      // segundos no modo fast antes de slow

// ─── TX Power ─────────────────────────────────────────────────────────────────
#define BLE_TX_POWER_DBM 4 // válidos: -40,-20,-16,-12,-8,-4,0,4

// ─── Códigos de ação (Byte 0 do payload de 4 bytes) ──────────────────────────
#define CMD_LED_BLINK 0x01 // [0x01, hi_ms, lo_ms, 0x00]
#define CMD_BUZZ 0x02      // [0x02, hi_ms, lo_ms, 0x00]
#define CMD_BOTH 0x03      // [0x03, hi_ms, lo_ms, 0x00]
#define CMD_ALERT 0x04
#define CMD_STOP 0xFF // [0xFF, 0x00, 0x00, 0x00]