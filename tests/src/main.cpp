#include <Arduino.h>

#define BUTTON PIN_024
#define LED LED_BUILTIN

void setup()
{
   Serial.begin(115200);
   while (!Serial)
   {
      delay(10);
   }

   Serial.println("Connected to Serial");
   Serial.println("Connecting to Components...");
   pinMode(LED, OUTPUT);
   pinMode(BUTTON, INPUT_PULLDOWN);
}

auto ledState = LED_STATE_OFF;
auto buttonState = LOW;

void loop()
{
   buttonState = digitalRead(BUTTON);

   if (buttonState == HIGH)
   {
      ledState = !ledState;
      digitalWrite(LED, ledState);

      Serial.println("Button Pressed");

      while (digitalRead(BUTTON) == HIGH)
      {
         delay(10);
      }

      Serial.println("Button Released");
   }
}