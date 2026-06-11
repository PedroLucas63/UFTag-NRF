#pragma once

#include <stdint.h>
#include <stdbool.h>


#define KEY_LEN 32

void ksInit();
bool ksHasKey();
bool ksGet(uint8_t out[KEY_LEN]);
bool ksSave(const uint8_t key[KEY_LEN]);