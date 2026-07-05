#include "state-machine/StateMachine.h"
#include "ble/core/BleCore.h"
#include "ble/gatt/BleGatt.h"
#include "actuator/led/ActLed.h"
#include "actuator/buzzer/ActBuzz.h"
#include "actuator/button/ActButton.h"
#include "ble/store/KeyStore.h"
#include "actuator/ActConfig.h"
#include <Arduino.h>

StateMachine currentState = StateMachine::DISABLED;

void updateState(StateMachine newState)
{
   if (currentState != newState)
   {
      if (DEBUG)
      {
         Serial.printf("[DEBUG] State Transition: %d -> %d\n", static_cast<int>(currentState), static_cast<int>(newState));
      }
      currentState = newState;
   }
}

void init_device()
{
   if (DEBUG)
   {
      Serial.println("\n--- [DEBUG] Inicializando UFTag ---");
   }

   actLedInit();
   btnInit();
   actBuzzInit();
   ksInit();
   bleInit();
   gattServiceInit();

   // Sinalização de inicialização (5 piscadas rápidas no LED vermelho)
   actLedPlayInitSinalization();

   if (ksHasKey())
   {
      updateState(StateMachine::ADVERTISING);
   }
   else
   {
      updateState(StateMachine::PAIRING);
   }

   if (DEBUG)
   {
      Serial.println("[DEBUG] Setup completo e rodando!");
   }
}

void advertising()
{
   static bool advStarted = false;
   if (!advStarted)
   {
      if (DEBUG)
      {
         Serial.println("[DEBUG] Iniciando advertising normal...");
      }
      bleAdvertisingStartNormal();
      advStarted = true;
   }
}

void pairing()
{
   static bool pairingStarted = false;
   if (!pairingStarted)
   {
      if (DEBUG)
      {
         Serial.println("[DEBUG] Iniciando advertising de pareamento...");
      }
      bleAdvertisingStartPairing();
      pairingStarted = true;
   }
}

void connected()
{
   // Estado conectado gerenciado por eventos/callbacks do Bluetooth
}

void buzzing()
{
   // Estado de alerta sonoro gerenciado pela tick do buzzer
}

void configuring()
{
   // Estado de configuração
}

void error()
{
   if (DEBUG)
   {
      Serial.println("[ERROR] Dispositivo entrou em estado de erro. Sinalizando e reiniciando para INIT...");
   }

   // Sinalização de erro: 3 piscadas longas (500ms aceso, 500ms apagado)
   for (int i = 0; i < 3; i++)
   {
      actLedSetRed(true);
      delay(500);
      actLedSetRed(false);
      delay(500);
   }

   // Reinicia transicionando de volta para INIT
   updateState(StateMachine::INIT);
}

void run()
{
   switch (currentState)
   {
   case StateMachine::DISABLED:
      // Apenas lê o botão para habilitar
      btnTick();
      break;

   case StateMachine::INIT:
      init_device();
      break;

   case StateMachine::ADVERTISING:
      advertising();
      break;

   case StateMachine::PAIRING:
      pairing();
      break;

   case StateMachine::CONNECTED:
      connected();
      break;

   case StateMachine::BUZZING:
      buzzing();
      break;

   case StateMachine::CONFIGURING:
      configuring();
      break;

   case StateMachine::ERROR:
      error();
      break;
   }

   // Executa as tarefas periódicas (ticks) para estados ativos
   if (currentState != StateMachine::DISABLED)
   {
      runLedTick();
      runBuzzTick();
      btnTick();
      bleTick();
   }

   // Pequeno delay para evitar sobrecarregar o processador nas tarefas de pooling
   if (currentState == StateMachine::ADVERTISING)
   {
      delay(200); // Delay maior em advertising para economizar energia
   }
   else
   {
      delay(20);  // Delay menor para garantir precisão das piscadas e botões em outros estados
   }
}