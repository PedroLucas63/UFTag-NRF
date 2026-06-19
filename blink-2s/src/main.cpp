#include <Arduino.h>
#include "ble/core/BleCore.h"
#include "ble/gatt/BleGatt.h"
#include "actuator/led/ActLed.h"
#include "actuator/buzzer/ActBuzz.h"
#include "actuator/button/ActButton.h"
#include "ble/store/KeyStore.h"
#include "actuator/ActConfig.h"

void setup() {
    Serial.begin(115200);
    delay(2000); 

    Serial.println("\n--- [DEBUG] Iniciando UFTag ---");

    Serial.println("[DEBUG] Inicializando LEDs...");
    actLedInit();
    
    Serial.println("[DEBUG] Inicializando Button...");
    btnInit();

    Serial.println("[DEBUG] Inicializando Buzzer...");
    actBuzzInit();

    Serial.println("[DEBUG] Inicializando KeyStore (Flash)...");
    ksInit();

    Serial.println("[DEBUG] Inicializando BLE Stack...");
    bleInit();

    Serial.println("[DEBUG] Inicializando GATT Services...");
    gattServiceInit();

    Serial.println("[DEBUG] Iniciando Advertising...");
    bleAdvertisingStart();

    Serial.println("UFTag - Setup completo e rodando!");
}

void loop() {
    actBuzzTick();
    actLedTick();
    btnTick();
}