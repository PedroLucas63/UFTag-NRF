#pragma once

#define DEVICE_NAME "UFTag"

#define BLE_SVC_UUID            "A1B2C3D4-E5F6-7890-ABCD-EF1234567890"
#define BLE_CHR_UUID            "A1B2C3D4-E5F6-7890-ABCD-EF1234567891"
#define BLE_CHR_SETKEY_UUID     "A1B2C3D4-E5F6-7890-ABCD-EF1234567892"  

// ─── Tamanho da chave pública (Curve25519) ────────────────────────────────────
#define BLE_PUB_KEY_LEN          32

// ─── Tamanho da chave pública (Curve25519) ────────────────────────────────────
#define BLE_COMPANY_ID_LO       0xFF
#define BLE_COMPANY_ID_HI       0xFF

// ─── Advertising ──────────────────────────────────────────────────────────────
#define BLE_ADV_FAST_INTERVAL   160     // 100ms  (unidade: 0.625ms)
#define BLE_ADV_SLOW_INTERVAL   800     // 500ms
#define BLE_ADV_FAST_TIMEOUT    30      // segundos no modo fast antes de slow

// ─── TX Power ─────────────────────────────────────────────────────────────────
#define BLE_TX_POWER_DBM        4       // válidos: -40,-20,-16,-12,-8,-4,0,4

// ─── Códigos de ação (Byte 0 do payload de 4 bytes) ──────────────────────────
#define CMD_LED_BLINK           0x01    // [0x01, hi_ms, lo_ms, 0x00]
#define CMD_BUZZ                0x02    // [0x02, hi_ms, lo_ms, 0x00]
#define CMD_BOTH                0x03    // [0x03, hi_ms, lo_ms, 0x00]
#define CMD_ALERT               0x04
#define CMD_STOP                0xFF    // [0xFF, 0x00, 0x00, 0x00]