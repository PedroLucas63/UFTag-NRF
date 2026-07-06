#pragma once

void bleInit();
void bleAdvertisingStart();
void bleAdvertisingStartPairing();
void bleAdvertisingStartNormal();
void bleAdvertisingSwitchType();

bool bleIsLostModeActive();
void bleSetLostModeState(bool active);
void bleTick();