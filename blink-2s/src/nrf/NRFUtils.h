#pragma once

#include <stdint.h>
#include <stdbool.h>


#define NRF_DEVICE_ID_LEN 16
#define NAME_LEN 16

void getDeviceId(char out_id[NRF_DEVICE_ID_LEN + 1]);

bool updateName(const uint8_t *name, uint16_t len);