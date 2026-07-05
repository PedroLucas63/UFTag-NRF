#pragma once

enum class StateMachine
{
   DISABLED,
   INIT,
   ADVERTISING,
   PAIRING,
   CONNECTED,
   BUZZING,
   CONFIGURING,
   ERROR,
};

extern StateMachine currentState;
const bool DEBUG = true;

void updateState(StateMachine newState);

void run();
void init_device();
void advertising();
void pairing();
void connected();
void buzzing();
void configuring();
void error();