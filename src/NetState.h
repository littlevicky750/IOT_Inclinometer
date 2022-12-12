#ifndef NetState_H
#define NetState_H
#include <Arduino.h>

class NetState
{
public:
    byte now = 1;
    const byte Off = 0;
    const byte On = 1;
    const byte ConnectingWifi = 2;
    const byte ConnectingMqtt = 3;
    const byte Finish = 4;
    int SendPackage = 0;
    const byte SignalScale[4] = {50, 15, 5, 0};
    byte Signal = 0;
    int Channel = 2;
    bool Swich = false;
};

#endif