#ifndef AngleCalculate_H
#define AngleCalculate_H
#include <Arduino.h>
#include "LEDFlash.h"


class AngleCalculate
{
private:
    float StableAngle[2] = {200};
    float MeasureAngle[2] = {0};
    size_t ArraySize= sizeof(StableAngle);
    byte StableCount = 0;

public:
    const byte Sleep = 0;
    const byte Not_Stable = 1;
    const byte Measuring = 2;
    const byte Done = 3;
    byte MeasureCount = 0;
    byte State = 0;
    LEDFlash *pLED;
    float ResultAngle[2] = {0};

    void Switch(bool OnOff)
    {
        if (OnOff && (State == Sleep || State == Done))
        {
            State = Not_Stable;
            memset(StableAngle,200,ArraySize);
            memset(MeasureAngle,0,ArraySize);
            StableCount = 0;
            MeasureCount = 0;
            pLED->Set(0,pLED->B,2,2);
        }
        if (!OnOff)
        {
            State = Sleep;
            memset(StableAngle,200,ArraySize);
            memset(MeasureAngle,0,ArraySize);
            StableCount = 0;
            MeasureCount = 0;
            pLED->Set(0,pLED->K,0,2);
        }
    }

    byte Input(float Angle0, float Angle1)
    {
        if (State == Sleep || State == Done)
        {
            return State;
        }
        if (StableCount < 5 || abs(StableAngle[0] - Angle0) > 0.1 || abs(StableAngle[1] - Angle1) > 0.1)
        {
            StableCount++;
            if (abs(StableAngle[0] - Angle0) > 0.05 || abs(StableAngle[1] - Angle1) > 0.05)
            {
                StableAngle[0] = Angle0;
                StableAngle[1] = Angle1;
                StableCount = 0;
            }
            memset(MeasureAngle,0,ArraySize);
            MeasureCount = 0;
            State = Not_Stable;
            pLED->Set(0,pLED->B,2,2);
            return Not_Stable;
        }
        else
        {
            MeasureAngle[0] += Angle0;
            MeasureAngle[1] += Angle1;
            MeasureCount++;
            if (MeasureCount == 10)
            {
                ResultAngle[0] = MeasureAngle[0] / MeasureCount;
                ResultAngle[1] = MeasureAngle[1] / MeasureCount;
                memset(StableAngle,200,ArraySize);
                memset(MeasureAngle,0,ArraySize);
                StableCount = 0;
                MeasureCount = 0;
                State = Done;
                pLED->Set(0,pLED->G,1,2);
                return Done;
            }
            State = Measuring;
            pLED->Set(0,pLED->B,1,2);
            return Measuring;
        }
    }
};

#endif