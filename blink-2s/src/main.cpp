#include <Arduino.h>

#define LED LED_BUILTIN
#define DELAY 5000

void setup() {
    pinMode(LED, OUTPUT);
}

void loop() {
    digitalWrite(LED, LOW);
    delay(DELAY);

    digitalWrite(LED, HIGH);
    delay(DELAY);
}
