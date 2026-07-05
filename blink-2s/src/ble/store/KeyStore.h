#pragma once

#include <stdint.h>
#include <stdbool.h>


#define KEY_LEN 32

void ksInit();
bool ksHasKey();
bool ksGet(uint8_t out[KEY_LEN]);
bool ksSave(const uint8_t key[KEY_LEN]);
void ksRemove();

bool ksGetName(char out_name[17]);
bool ksSaveName(const uint8_t *name, uint16_t len);
void ksRemoveName();
