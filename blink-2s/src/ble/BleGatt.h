#pragma once
#include <bluefruit.h>

// GAPP (Generic Attribute Profile) protocolo de comunicação entre cliente e servidor BLE

void        gattServiceInit();
BLEService& gattGetService();   // retorna referência para o advertising incluir esse serviço