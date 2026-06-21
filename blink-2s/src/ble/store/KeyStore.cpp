#include "KeyStore.h"
#include <InternalFileSystem.h>
#include <Arduino.h>

using namespace Adafruit_LittleFS_Namespace;

static const char *FILE_PATH = "/uftag_public_key.bin";
static uint8_t publicKey[KEY_LEN] = {0};
static bool hasKey = false;

void ksInit()
{
    if (!InternalFS.begin())
    {
        return;
    }

    File file = InternalFS.open(FILE_PATH, Adafruit_LittleFS_Namespace::FILE_O_READ);

    if (!file)
    {
        Serial.println("[DEBUG] No public key found in flash.");
        return;
    }

    Serial.println("[DEBUG] Public key found in flash, loading...");

    uint16_t bytesRead = file.read(publicKey, KEY_LEN);
    file.close();

    if (bytesRead == KEY_LEN)
    {
        hasKey = true;
    }
    else
    {
        memset(publicKey, 0, KEY_LEN);
    }
}

bool ksHasKey()
{
    return hasKey;
}

bool ksGet(uint8_t out[KEY_LEN])
{
    if (!hasKey)
        return false;
    memcpy(out, publicKey, KEY_LEN);
    return true;
}

void ksRemove()
{
    if (InternalFS.exists(FILE_PATH))
    {
        InternalFS.remove(FILE_PATH);
    }
    memset(publicKey, 0, KEY_LEN);
    hasKey = false;
}

bool ksSave(const uint8_t key[KEY_LEN])
{
    File file = InternalFS.open(FILE_PATH, Adafruit_LittleFS_Namespace::FILE_O_WRITE);
    if (!file)
        return false;

    Serial.println("[DEBUG] Saving public key to flash...");

    uint16_t written = file.write(key, KEY_LEN);
    file.close();

    if (written != KEY_LEN)
        return false;

    memcpy(publicKey, key, KEY_LEN);
    hasKey = true;

    return true;
}