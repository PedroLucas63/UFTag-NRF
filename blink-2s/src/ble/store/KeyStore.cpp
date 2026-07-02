#include "KeyStore.h"
#include <InternalFileSystem.h>
#include <Arduino.h>

using namespace Adafruit_LittleFS_Namespace;

static const char *FILE_PATH = "/uftag_public_key.bin";
static const char *NAME_FILE_PATH = "/uftag_device_name.bin";
static const char *LOST_FILE_PATH = "/uftag_lost_state.bin";

static uint8_t publicKey[KEY_LEN] = {0};
static bool hasKey = false;

static bool lostState = false;

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

    // Carrega o estado de lost da flash
    File lostFile = InternalFS.open(LOST_FILE_PATH, Adafruit_LittleFS_Namespace::FILE_O_READ);
    if (lostFile)
    {
        uint8_t temp = 0;
        if (lostFile.read(&temp, 1) == 1)
        {
            lostState = (temp != 0);
        }
        lostFile.close();
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
    ksRemoveName();

    if (InternalFS.exists(LOST_FILE_PATH))
    {
        InternalFS.remove(LOST_FILE_PATH);
    }
    lostState = false;
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

bool ksGetName(char out_name[17])
{
    if (!InternalFS.begin())
    {
        return false;
    }

    File file = InternalFS.open(NAME_FILE_PATH, Adafruit_LittleFS_Namespace::FILE_O_READ);
    if (!file)
    {
        return false;
    }

    int bytesRead = file.read(out_name, 16);
    file.close();

    if (bytesRead > 0 && bytesRead <= 16)
    {
        out_name[bytesRead] = '\0';
        return true;
    }
    return false;
}

bool ksSaveName(const uint8_t *name, uint16_t len)
{
    if (len > 16) return false;

    File file = InternalFS.open(NAME_FILE_PATH, Adafruit_LittleFS_Namespace::FILE_O_WRITE);
    if (!file)
        return false;

    uint16_t written = file.write(name, len);
    file.close();

    return (written == len);
}

void ksRemoveName()
{
    if (InternalFS.exists(NAME_FILE_PATH))
    {
        InternalFS.remove(NAME_FILE_PATH);
    }
}


bool ksGetLostState()
{
    return lostState;
}

void ksSaveLostState(bool state)
{
    lostState = state;
    File file = InternalFS.open(LOST_FILE_PATH, Adafruit_LittleFS_Namespace::FILE_O_WRITE);
    if (file)
    {
        uint8_t temp = state ? 1 : 0;
        file.write(&temp, 1);
        file.close();
    }
}
