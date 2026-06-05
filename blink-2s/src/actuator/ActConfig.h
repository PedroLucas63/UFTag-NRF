#pragma once
#include <Arduino.h>

// ─── Pinos (nice!nano v2) ─────────────────────────────────────────────────────
#define ACT_PIN_LED_RED         LED_BUILTIN 
#define ACT_PIN_LED_BLUE        PIN_114     
#define ACT_PIN_BUZZER          20          
#define ACT_PIN_BUTTON          24       

// ─── LED (ativo LOW no nice!nano) ────────────────────────────────────────────
#define ACT_LED_ON              LOW
#define ACT_LED_OFF             HIGH

// ─── Buzzer ───────────────────────────────────────────────────────────────────
#define ACT_BUZZ_FREQ_HZ        2000        // 2kHz — tom audível padrão