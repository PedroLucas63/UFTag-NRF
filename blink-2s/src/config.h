#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// --- Configurações do BLE ---
#define DEVICE_NAME_PREFIX "UFTag-"
#define UFTAG_UUID         "12345678-1234-1234-1234-123456789abc"

// Códigos de Ação do GATT Command (Fase 3)
#define CMD_OFF         0x00
#define CMD_LED_ON      0x01
#define CMD_LED_BLINK   0x02
#define CMD_BUZZ        0x03
#define CMD_BUZZ_BLINK  0x04

#endif