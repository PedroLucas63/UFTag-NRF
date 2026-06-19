#pragma once
#include <Arduino.h>

// ─── Pinos (nice!nano v2) ─────────────────────────────────────────────────────
#define ACT_PIN_LED_RED         LED_BUILTIN 
#define ACT_PIN_LED_BLUE        PIN_114     
#define ACT_PIN_BUZZER          20          
#define ACT_PIN_BUTTON          24       

// ─── LED (ativo LOW no nice!nano) ────────────────────────────────────────────
#define ACT_LED_ON              HIGH
#define ACT_LED_OFF             LOW

// ─── Buzzer ───────────────────────────────────────────────────────────────────
#define ACT_BUZZ_FREQ_HZ        2000        // 2kHz — tom audível padrão

// ─── Button ───────────────────────────────────────────────────────────────────
#define HOLD_TO_SLEEP_MS  5000
