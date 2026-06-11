#include "KeyStore.h"
#include <InternalFileSystem.h>
#include <Arduino.h>

using namespace Adafruit_LittleFS_Namespace;

static const char* FILE_PATH = "/uftag_public_key.bin";
static uint8_t publicKey[KEY_LEN] = {0};
static bool hasKey = false;

void ksInit(){
    if(!InternalFS.begin()){
        return;
    }

    File file = InternalFS.open(FILE_PATH, Adafruit_LittleFS_Namespace::FILE_O_READ);

    if(!file) return;

    uint16_t bytesReade = file.read(publicKey, KEY_LEN);
    file.close();
    
    if(bytesReade == KEY_LEN){
        hasKey = true;
    }else{
        memset(publicKey, 0, KEY_LEN);
    }

}


bool ksHasKey(){
    return hasKey;
}


bool ksGet(uint8_t out[KEY_LEN]){
    if(!hasKey) return false;
    memcpy(out, publicKey, KEY_LEN);
    return true;
}


bool ksSave(const uint8_t key[KEY_LEN]){

    if(InternalFS.exists(FILE_PATH)){
        InternalFS.remove(FILE_PATH);
    }

    File file = InternalFS.open(FILE_PATH, Adafruit_LittleFS_Namespace::FILE_O_WRITE);
    if(!file) return false;

    uint16_t written = file.write(key, KEY_LEN);
    file.close();

    if(written != KEY_LEN) return false;

    memcpy(publicKey, key, KEY_LEN);
    hasKey = true;

    return true;

}