#pragma once

#define NRF_DEVICE_ID_LEN 16

void getDeviceId(char out_id[NRF_DEVICE_ID_LEN + 1]);