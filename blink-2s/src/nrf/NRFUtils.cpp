#include "NRFUtils.h"
#include <nrf.h>
#include <stdio.h>

void getDeviceId(char out_id[NRF_DEVICE_ID_LEN + 1])
{
   uint32_t id0 = NRF_FICR->DEVICEID[0];
   uint32_t id1 = NRF_FICR->DEVICEID[1];
   snprintf(out_id, 17, "%08lX%08lX", id0, id1);
}