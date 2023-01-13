#ifndef NET_H
#define NET_H

#include <WiFi.h>
#include <esp_wifi.h>
extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}
#include <AsyncMqttClient.h>
#include "SerialDebug.h"
#include "NetState.h"

#define WIFI_SSID "GH034"
#define WIFI_PASSWORD "chenlaoshiniubi"
// Raspberry Pi Mosquitto MQTT Broker
#define MQTT_HOST IPAddress(192, 168, 10, 149)
// For a cloud MQTT broker, type the domain name
// #define MQTT_HOST "example.com"
#define MQTT_PORT 1883
// Temperature MQTT Topic
char MQTT_PUB_realtime[23] = "esp32_2/wcimu/realtime";
char MQTT_PUB_measurement[26] = "esp32_2/wcimu/measurement";
RTC_DATA_ATTR int WiFi_Channel = 2;
bool isSendingMeasure = false;

class MQTTMsgBuffer
{
private:
    String Msg[5];
    int C = 0;
    int Start = 0;

public:
    void Add(String &AddMsg)
    {
        if (AddMsg != "")
        {
            Msg[C] = AddMsg;
            C++;
            C %= 5;
        }
    }
    String Take()
    {
        if (Start == C)
            return "";
        String Temp = Msg[Start];
        Msg[Start] = "";
        Start++;
        Start %= 5;
        return Temp;
    }
};

NetState WiFiState;
MQTTMsgBuffer MsgBuffer;

AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;
TimerHandle_t wifiOnOffTimer;

void Net_Send_Realtime(String &Msg)
{
    if (WiFiState.now == WiFiState.Finish && !isSendingMeasure)
        WiFiState.SendPackage = mqttClient.publish(MQTT_PUB_realtime, 1, true, Msg.c_str());
}

void Net_Send_Measure(String Msg)
{

    if (WiFiState.now != WiFiState.Finish)
    {
        MsgBuffer.Add(Msg);
    }
    else
    {
        isSendingMeasure = true;
        String Temp = MsgBuffer.Take();
        while (Temp != "")
        {
            WiFiState.SendPackage = mqttClient.publish(MQTT_PUB_measurement, 1, true, Temp.c_str());
            Temp = MsgBuffer.Take();
            delay(1000);
        }
        if (Msg != "")
        {
            WiFiState.SendPackage = mqttClient.publish(MQTT_PUB_measurement, 1, true, Msg.c_str());
            delay(1000);
        }
        isSendingMeasure = false;
    }
}

void onMqttConnect(bool sessionPresent)
{
    Debug.println("[MQTT] Connected to MQTT. ");
    WiFiState.now = WiFiState.Finish;
    WiFiState.Signal = 1;
    Net_Send_Measure("");
    LED.Set(1, LED.G, 20, 1);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
    if (WiFiState.now == WiFiState.Finish)
        Debug.println("[MQTT] Disconnected from MQTT.");
    WiFiState.Signal = 0;
    if (WiFi.isConnected())
    {
        xTimerStart(mqttReconnectTimer, 0);
        WiFiState.now = WiFiState.ConnectingMqtt;
    }
    LED.Set(1, 0, 0, 1);
}

void onMqttPublish(uint16_t packetId)
{
    if (WiFiState.Signal != 1 && packetId - WiFiState.SendPackage > WiFiState.SignalScale[WiFiState.Signal - 2])
        WiFiState.Signal -= 1;
    else if (packetId - WiFiState.SendPackage < WiFiState.SignalScale[WiFiState.Signal - 1])
        WiFiState.Signal += 1;
}

void connectToWifi()
{
    Serial.println("Connecting to Wi-Fi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void connectToMqtt()
{
    Serial.println("Connecting to MQTT...");
    mqttClient.connect();
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
        xTimerStop(mqttReconnectTimer, 0);
        xTimerStop(wifiReconnectTimer, 0);
        Debug.println("[WiFi] Wifi Off.");
        break;
    case SYSTEM_EVENT_STA_CONNECTED: // 4
        xTimerStop(wifiReconnectTimer, 0);
        WiFiState.now = WiFiState.ConnectingWifi;
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED: // 5
        if (WiFiState.now >= WiFiState.ConnectingMqtt)
            Debug.println("[WiFi] Wifi disconnect.");
        if (WiFiState.now != WiFiState.Off)
        {
            WiFiState.now = WiFiState.On;
            xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
            xTimerStart(wifiReconnectTimer, 0);
        }
        break;
    case SYSTEM_EVENT_STA_GOT_IP: // 7
        Debug.println("[WiFi] Wifi connected. IP Address: " + String(WiFi.localIP(), HEX));
        xTimerStart(mqttReconnectTimer, 0);
        WiFiState.now = WiFiState.ConnectingMqtt;
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
            char A = "012345"[WiFiState.Channel];
            MQTT_PUB_realtime[6] = A;
            MQTT_PUB_measurement[6] = A;
            Serial.println(MQTT_PUB_realtime);
            WiFi_Channel = WiFiState.Channel;
        }
    }
}

void Net_Init()
{
    WiFiState.Channel = WiFi_Channel;
    char A = "012345"[WiFiState.Channel];
    MQTT_PUB_realtime[6] = A;
    MQTT_PUB_measurement[6] = A;
    WiFi.disconnect(true);
    // pdFALSE : timer will be a one-shot timer and enter the dormant state after it expires.
    // ID = 0 : make sure "Connect to Wifi" and "Connect to MTQQ" won't execute at the same time and cause crashing.
    // Timer Call Back will run in the core same as Net_Init was called.
    mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
    wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));
    wifiOnOffTimer = xTimerCreate("WiFiOnOffTimer", pdMS_TO_TICKS(1000), pdFALSE, (void *)0, reinterpret_cast<TimerCallbackFunction_t>(WiFiSwich));
    WiFi.onEvent(WiFiEvent);

    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    // mqttClient.onSubscribe(onMqttSubscribe);
    // mqttClient.onUnsubscribe(onMqttUnsubscribe);
    mqttClient.onPublish(onMqttPublish);
    mqttClient.setServer(MQTT_HOST, MQTT_PORT);
    // If your broker requires authentication (username and password), set them below
    // mqttClient.setCredentials("REPlACE_WITH_YOUR_USER", "REPLACE_WITH_YOUR_PASSWORD");
    connectToWifi();
}

#endif
