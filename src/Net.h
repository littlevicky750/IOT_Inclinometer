#ifndef NET_H
#define NET_H

#include <WiFi.h>
#include <esp_wifi.h>
#include <HTTPClient.h>
#include "SerialDebug.h"
#include "NetState.h"
#include <Preferences.h>
const char *ntpServer = "pool.ntp.org";
int gmtOffset = 8;
const int daylightOffset_sec = 0;
Preferences preferences;

String ssid = "null";                                                                                   // change SSID
String password = "null";                                                                               // change password
String sheet_id = "1tF6z2MT1cIY5qWEmZyBdOPa5mv495GyZaV8sPPVWAIk";                                       // change Google Sheet ID
String GOOGLE_SCRIPT_ID = "AKfycbx94EAb_avqk2NM4sumYtZEbrkhBkzCi_SKkA953yyg8ZSdCcV5u-8aucB-jn0W7Ub8zQ"; // change Google Sheet App Script ID

bool Require_Time_Adjust = true;
bool isWifiWorking = false;
int httpCode_pre = 0;
unsigned int ErrorCount = 0;

NetState WiFiState;

TimerHandle_t wifiReconnectTimer;
TimerHandle_t wifiOnOffTimer;

class MsgBuffer
{
private:
    const unsigned int MsgBufferSize = 10;
    String Msg[10];
    int C = 0;
    int Start = 0;

public:
    void print_now()
    {
        Serial.println("==================================================");
        for (int i = 0; i < MsgBufferSize; i++)
        {
            if (i == Start)
                Serial.print(">|");
            else if (i == C)
                Serial.print("+|");
            else
                Serial.print("||");
            Serial.println(Msg[i]);
        }
    }

    String Take()
    {
        return Msg[Start];
    }
    void Next()
    {
        Msg[Start] = "";
        Start = (Start + 1) % MsgBufferSize;
        // print_now();
    }
    void Add(String &AddMsg)
    {
        if (AddMsg != "")
        {
            Msg[C] = AddMsg;
            C = (C + 1) % MsgBufferSize;
            if (C == Start)
                Next();
            // print_now();
        }
    }
};

MsgBuffer Net_Send_Msg_Buffer;

void GetNPTTime()
{
    Debug.println("[NPT] Time Zone = GMT+" + String(gmtOffset));
    configTime(gmtOffset * 3600, daylightOffset_sec, ntpServer);
    delay(10000);
    struct tm A;
    getLocalTime(&A);
    int TimeOffset = Clock.CheckTimeDifference(A.tm_year + 1900, A.tm_mon + 1, A.tm_mday, A.tm_hour, A.tm_min, A.tm_sec);
    if (abs(TimeOffset - gmtOffset * 3600) < 100)
        Require_Time_Adjust = true;
    else
        Clock.SetTime(A.tm_year + 1900, A.tm_mon + 1, A.tm_mday, A.tm_hour, A.tm_min, A.tm_sec);
}

void Net_Signal_Check()
{
    if (isWifiWorking)
        return;
    isWifiWorking = true;
    WiFiState.Signal = 4 + (int)WiFi.RSSI() / 32;
    if (Require_Time_Adjust && WiFiState.now == WiFiState.Connected)
    {
        delay(1000);
        GetNPTTime();
        Require_Time_Adjust = false;
    }
    isWifiWorking = false;
}

bool Net_Send(String Data)
{
    String urlFinal = "";
    urlFinal += "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?";
    urlFinal += "id=" + sheet_id;
    urlFinal += "&sheet=test" + String(WiFiState.Channel);
    urlFinal += Data;
    // Debug.println("POST data to spreadsheet : " + urlFinal);
    WiFiClient client;
    HTTPClient http;
    http.begin(urlFinal.c_str());
    http.setConnectTimeout(10000);
    http.setTimeout(10000);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

    // Check response from Apps script
    bool SendSuccess = true;
    int httpCode = http.GET();
    if (httpCode > 0)
    {
        String payload = http.getString();
        int Get_return_id = payload.indexOf("return");
        int Get_return = atoi(payload.substring(Get_return_id + 8).c_str());
        if (Get_return_id == -1 || Get_return != 1)
        {
            Debug.print("[WiFi] Data Upload Failed >> " + payload);
            SendSuccess = false;
        }
    }
    else
    {
        if (httpCode == -1)
        {
            if (httpCode != httpCode_pre)
                Debug.println("[WiFi] Upload Error >> " + http.errorToString(httpCode));
            SendSuccess = false;
        }
        else if (httpCode != httpCode_pre)
            Serial.println("[WiFi] Upload Error >> " + http.errorToString(httpCode));
    }
    httpCode_pre = httpCode;
    http.end();
    return SendSuccess;
}

bool Net_Send_Measure(String Data)
{
    if (Net_Send_Msg_Buffer.Take() == "" && Data == "")
    {
        return false;
    }
    if (WiFi.status() != WL_CONNECTED || isWifiWorking)
    {
        Net_Send_Msg_Buffer.Add(Data);
        return false;
    }
    isWifiWorking = true;
    if (Net_Send_Msg_Buffer.Take() != "")
    {
        bool SendSuccess = Net_Send(Net_Send_Msg_Buffer.Take());
        if (SendSuccess)
            Net_Send_Msg_Buffer.Next();
        else
        {
            ErrorCount++;
            if (ErrorCount > 5)
            {
                Net_Send_Msg_Buffer.Next();
                ErrorCount = 0;
            }
        }
        Net_Send_Msg_Buffer.Add(Data);
    }
    else
    {
        if (!Net_Send(Data))
            Net_Send_Msg_Buffer.Add(Data);
    }
    isWifiWorking = false;
    return true;
}

void connectToWifi()
{
    // Serial.println("Connecting to Wi-Fi...");
    WiFi.begin(ssid.c_str(), password.c_str());
}

void reconnectToWiFi()
{
    if (WiFiState.now == WiFiState.Connected)
    {
        WiFi.disconnect(true, true);
    }
    xTimerStart(wifiReconnectTimer, 0);
}

void WiFiEvent(WiFiEvent_t event)
{
    // Wifi Event always work in core 1. Used timer to control connecting event.
    // Serial.printf("[WiFi-event] event: %d. \n", event);
    switch (event)
    {
    case SYSTEM_EVENT_SCAN_DONE: // 1
        xTimerStart(wifiReconnectTimer, 0);
        break;
    case SYSTEM_EVENT_STA_START: // 2
        if (WiFiState.now == WiFiState.Off)
            xTimerStart(wifiReconnectTimer, 0);
        WiFiState.now = WiFiState.On;
        Debug.println("[WiFi] Wifi On.");
        break;
    case SYSTEM_EVENT_STA_STOP: // 3
        WiFiState.now = WiFiState.Off;
        xTimerStop(wifiReconnectTimer, 0);
        Debug.println("[WiFi] Wifi Off.");
        break;
    case SYSTEM_EVENT_STA_CONNECTED: // 4
        xTimerStop(wifiReconnectTimer, 0);
        WiFiState.now = WiFiState.ConnectingWifi;
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED: // 5
        if (WiFiState.now >= WiFiState.Connected)
            Debug.println("[WiFi] Wifi disconnect.");
        if (WiFiState.now != WiFiState.Off)
        {
            WiFiState.now = WiFiState.On;
            xTimerStart(wifiReconnectTimer, 0);
        }
        break;
    case SYSTEM_EVENT_STA_GOT_IP: // 7
        Debug.println("[WiFi] Wifi connected. IP Address: " + String(WiFi.localIP(), HEX));
        WiFiState.now = WiFiState.Connected;
        break;
    }
}

void WiFiSwich()
{
    if (WiFiState.now != WiFiState.Off)
        esp_wifi_stop();
    else
        esp_wifi_start();
}

void WiFiChannel(int k)
{
    byte PrevioudState = WiFiState.Channel;
    WiFiState.Channel += k;
    WiFiState.Channel = (WiFiState.Channel < 0) ? 0 : (WiFiState.Channel > 5) ? 5
                                                                              : WiFiState.Channel;
    if (PrevioudState != WiFiState.Channel)
    {
        if (PrevioudState == 0 && WiFiState.now == WiFiState.Off)
            xTimerStart(wifiOnOffTimer, 0);
        if (WiFiState.Channel == 0 && WiFiState.now != WiFiState.Off)
            xTimerStart(wifiOnOffTimer, 0);
        if (WiFiState.Channel != 0)
        {
            preferences.begin("my-app", false);
            preferences.putInt("WiFiChannel", WiFiState.Channel);
            preferences.end();
        }
    }
}

void Net_Init()
{
    preferences.begin("my-app", false);
    WiFiState.Channel = preferences.getInt("WiFiChannel", 1);
    preferences.end();
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true, true);
    // pdFALSE : timer will be a one-shot timer and enter the dormant state after it expires.
    // ID = 0 : make sure task won't execute at the same time and cause crashing.
    // Timer Call Back will run in the core same as Net_Init was called.
    wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));
    wifiOnOffTimer = xTimerCreate("WiFiOnOffTimer", pdMS_TO_TICKS(1000), pdFALSE, (void *)0, reinterpret_cast<TimerCallbackFunction_t>(WiFiSwich));
    WiFi.onEvent(WiFiEvent);
    connectToWifi();
}

void Net_Set(String Info)
{
    if (Info != "")
    {
        String title[5] = {"ssid=", "password=", "script_id=", "sheet_id=", "TimeZone=GMT+"};
        String Info_Cut[5];
        bool Info_Error = false;
        for (int i = 0; i < 5; i++)
        {
            int Info_Start = Info.indexOf(title[i]);
            int Info_End = Info.substring(Info_Start).indexOf("\n");
            if (Info_Start == -1)
                Info_Cut[i] == "";
            else if (Info_End == -1)
                Info_Cut[i] = Info.substring(Info_Start + title[i].length());
            else
                Info_Cut[i] = Info.substring(Info_Start + title[i].length(), Info_End + Info_Start - 1);
        }
        if (!Info_Error)
        {
            if (ssid != Info_Cut[0] || password != Info_Cut[1])
            {
                ssid = Info_Cut[0];
                password = Info_Cut[1];
                reconnectToWiFi();
            }
            if (Info_Cut[2] != "")
                GOOGLE_SCRIPT_ID = Info_Cut[2];
            if (Info_Cut[3] != "")
                sheet_id = Info_Cut[3];
            if (Info_Cut[4] != "")
            {
                int NewTime = atoi(Info_Cut[4].c_str());
                if (NewTime != gmtOffset)
                {
                    gmtOffset = NewTime;
                    WiFiState.GMT = NewTime;
                    Require_Time_Adjust = true;
                }
            }
        }
    }
}
#endif
