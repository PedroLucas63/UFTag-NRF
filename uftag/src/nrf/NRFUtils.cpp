#include "NRFUtils.h"
#include <nrf.h>
#include <stdio.h>
#include <bluefruit.h>
#include "ble/store/KeyStore.h"


void getDeviceId(char out_id[NRF_DEVICE_ID_LEN + 1])
{
   uint32_t id0 = NRF_FICR->DEVICEID[0];
   uint32_t id1 = NRF_FICR->DEVICEID[1];
   snprintf(out_id, 17, "%08lX%08lX", id0, id1);
}


bool updateName(const uint8_t *name, uint16_t len) {
   char newName[NAME_LEN + 1];
   uint16_t copyLen = (len < NAME_LEN) ? len : NAME_LEN;
   memcpy(newName, name, copyLen);
   newName[copyLen] = '\0'; // Garante terminação nula

   Bluefruit.setName(newName);
   ksSaveName(name, copyLen);
   return true;
}
