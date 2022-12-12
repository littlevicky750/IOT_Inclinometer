#ifndef LongPressSwich_H
#define LongPressSwich_H

#include <Arduino.h>
#include "OLED.h"
#include "SDCard.h"
#include "SerialDebug.h"
#include "LEDFlash.h"
#include "OLED.h"
#include "Battery.h"
RTC_DATA_ATTR int bootCount = -1;

class LongPressSwich
{
private:
    int LEDFlashCount = 0;
    int OffClock = 0;
    byte SWPin;
    byte ButPin;
    LEDFlash *pLED;
    Battery *pBattery;
    OLED *pOLED;

public:
    SDCard *pSD;
    int *LastTriggure;
    void On(gpio_num_t WakeUpPin, byte Swich_Pin, LEDFlash &LED, Battery &Bat, OLED &oled)
    {
        esp_sleep_enable_ext0_wakeup(WakeUpPin, 0);
        bootCount++;

        SWPin = Swich_Pin;
        pinMode(SWPin, OUTPUT);
        digitalWrite(SWPin, HIGH);
        // Begine from sleeping
        if (bootCount == 0)
        {
            oled.Clear();
            esp_deep_sleep_start();
        }

        // Setting
        Serial.setRxBufferSize(256);
        Serial.begin(115200);
        pOLED = &oled;
        pBattery = &Bat;
        pLED = &LED;
        ButPin = WakeUpPin;
        // Battary Test
        pBattery->Update();

        if (!TestVersion)
        {
            if (pBattery->Percent < 0)
            {
                pLED->Set(0, LED.R, 1, 4);
                pLED->Update();
                pOLED->ShowLowPower();
                while (!digitalRead(ButPin))
                    delay(10);
                esp_deep_sleep_start();
            }
        }

        pLED->Set(0, LED.W, 1, 4);
        pLED->Update();
        // Detect 3 s long press
        pinMode(ButPin, INPUT);
        while (millis() < 3000 && !digitalRead(ButPin))
        {
            delay(100);
        }
        if (millis() < 3000)
        {
            pOLED->Clear();
            digitalWrite(SWPin, LOW);
            esp_deep_sleep_start();
        }
    }

    void Off_Clock_Start()
    {
        OffClock = millis();
    }

    void Off_Clock_Stop()
    {
        OffClock = 0;
    }

    void Off_Clock_Check()
    {
        bool PressSleep = false;
        if (OffClock == 0)
            PressSleep = false;
        else if (millis() - OffClock > 3000)
            PressSleep = true;
        else
            pLED->Set(0, pLED->W, 1, 4);

        bool TimeOffSleep = (millis() - *LastTriggure > 5 * 60 * 1000) && (OffClock == 0);
        bool LowPowerOff = (pBattery->Percent < 0);

        if (!PressSleep && !TimeOffSleep && !LowPowerOff)
        {
            return;
        }
        if (TestVersion)
        {
            LowPowerOff = false;
        }

        if (PressSleep)
        {
            pOLED->Block("Shutting Down");
            Debug.println("Command Sleep");
        }
        else if (LowPowerOff)
        {
            pOLED->Block("Low Battery");
            Debug.println("Low Battery Sleep");
        }
        else if (TimeOffSleep)
        {
            pOLED->Block("Auto Sleep");
            Debug.println("Auto Sleep");
        }

        if (PressSleep || TimeOffSleep || LowPowerOff)
        {
            cli();
            int ForShow = millis();
            if (pSD)
            {
                // Save Debug message.
                String T = "";
                pSD->Save("", T);
            }
            while (millis() - ForShow < 2000)
            {
            }
            if (TimeOffSleep && millis() - *LastTriggure < 5000)
            {
                sei();
                return;
            }
            pOLED->TurnOff();
            while (digitalRead(ButPin) == 0)
            {
            }
            Serial.println("Sleep");
            digitalWrite(SWPin, LOW);
            esp_deep_sleep_start();
        }
    }
};
#endif