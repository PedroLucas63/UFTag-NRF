#pragma once

void bleInit();
void bleAdvertisingStart();
void bleAdvertisingStartPairing();
void bleAdvertisingStartNormal();
void bleAdvertisingSelectType();

bool bleIsLostModeActive();
void bleSetLostModeState(bool active);
void bleResetLostModeTimer();
void bleTick();