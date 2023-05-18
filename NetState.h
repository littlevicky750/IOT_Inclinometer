#ifndef NetState_H
#define NetState_H
#include <Arduino.h>

class NetState
{
public:
    byte now = 0;
    const byte Off = 0;
    const byte On = 1;
    const byte ConnectingWifi = 2;
    const byte Connected = 3;
    int SendPackage = 0;
    int GMT = 8;
    byte Signal = 0;
    int Channel = 2;
    bool Swich = false;
};

#endif