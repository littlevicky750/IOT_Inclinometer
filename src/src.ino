// Add upload to google sheet function
// Add set wifi name & password from SD card function
#include <Arduino.h>
#include "SerialDebug.h"

SerialDebug D;
extern SerialDebug Debug = D;

const gpio_num_t Pin_Button_Wakeup = GPIO_NUM_0;
const byte Pin_SwichEN = 1;
const byte Pin_Button0 = Pin_Button_Wakeup;
const byte Pin_Button1 = 7;
const byte Pin_Button2 = 6;
const byte Pin_I2C_SDA = 8; // Default
const byte Pin_I2C_SCL = 9; // Default
const byte Pin_SD_MOSI = 12;
const byte Pin_SD_MISO = 13;
const byte Pin_SD_SCK = 14;
const byte Pin_SD_CS = 15;
const byte IO_RFID_RST = 16;
const byte Pin_Battery = 17;
const byte IO_Buzzer = 18;
#define IO_OLED_RST 19
const byte Pin_IMU_ISR = 20;
const byte Pin_IMU_RX = 38;
const byte Pin_IMU_TX = 39;
const byte Pin_LED_1_B = 47;
const byte Pin_LED_1_G = 4;
const byte Pin_LED_1_R = 48;
const byte Pin_LED_0_B = 40;
const byte Pin_LED_0_G = 41;
const byte Pin_LED_0_R = 42;

/*
LED Setting
LED 0 :
  R-L - Bat Low Power (1)
  Y-L - IMU Not Warm Up (2)
  B-F - IMU Waiting for Stable (2)
  B-L - IMU Collecting Data (2)
  G-L - IMU Collect Data Done (2)
  M-L - RTC Lost Power (0)
  M-F - RTC Set Time Success (3)
  W-L - IO-0 Press (4)
  W-F - RFID Find New (5)
LED 1 :
  R-F - Any Other Error (0)
  G-F - File Save Success / Create New File (1)
  Y-F - No SD (1)
LED 0 + 1 :
  G-F - Working
*/

#define RFID_RST IO_RFID_RST
#define TestVersion false
#include "LEDFlash.h"
LEDFlash LED;

#include "SDCard.h"
#include "RealTimeClock.h"
RealTimeClock Clock;

#include "Net.h"
#include "OLED.h"
#include "LongPressSwich.h"
#include "IMU42688.h"
#include "Battery.h"
#include "RFID.h"
#include "AngleCalculate.h"
#include "Go_Standard.h"

TaskHandle_t *T_FAST;
TaskHandle_t *T_LOOP;
TaskHandle_t *T_MAIN;
TaskHandle_t *T_SLOW;
TaskHandle_t *T_SAVE;

IMU42688 imu;
OLED oled;
LongPressSwich Swich;
SDCard sdCard;

Battery Bat;
RFID rfid;
AngleCalculate Calculate;
Go_Standard Standard;

int MainLoopDelay = 250;
int FastLoopDelay = 125;
int LastEdit = 0;
bool fHaveSD = false;
bool fSave = false;
bool doRFID = true;

static void FAST(void *pvParameter)
{
  TickType_t xLastWakeTime;
  BaseType_t xWasDelayed;
  xLastWakeTime = xTaskGetTickCount();
  for (;;)
  {
    xWasDelayed = xTaskDelayUntil(&xLastWakeTime, FastLoopDelay);
    if (!xWasDelayed && millis() > 10000)
      Debug.println("[Warning] Task Fast Time Out.");
    if (imu.Update() == imu.IMU_Update_Success)
    {
      Calculate.Input(imu.getHorizontal(), imu.getVertical());
    }
    ButtonUpdate();
    oled.Update();
  }
}

static void MAIN(void *pvParameter)
{
  TickType_t xLastWakeTime;
  BaseType_t xWasDelayed;
  xLastWakeTime = xTaskGetTickCount();
  byte TimeOutCount = 0;
  for (;;)
  {
    xWasDelayed = xTaskDelayUntil(&xLastWakeTime, MainLoopDelay);
    if (!xWasDelayed && millis() > 10000)
    {
      TimeOutCount++;
      if (fHaveSD || TimeOutCount > 10)
      {
        Debug.println("[Warning] Main Loop Time Out.");
        TimeOutCount = 0;
      }
    }
    else
      TimeOutCount = 0;
    Swich.Off_Clock_Check();
    Clock.update();
    LED.Update();
    if (doRFID)
      rfid.Update();
  }
}

static void SLOW(void *pvParameter)
{
  for (;;)
  {
    Net_Signal_Check();
    Bat.Update();
    if (Bat.Percent < 10)
      LED.Set(0, LED.R, 1, 1);
    else
      LED.Set(0, 0, 0, 1);
    vTaskDelay(10000);
  }
}

static void Save(void *pvParameter)
{
  sdCard.SetPin(Pin_SD_SCK, Pin_SD_MISO, Pin_SD_MOSI, Pin_SD_CS);
  Net_Init();
  // vTaskDelay(4000);
  sdCard.Swich(true);
  TickType_t xLastWakeTime;
  BaseType_t xWasDelayed;
  xLastWakeTime = xTaskGetTickCount();
  bool do_WiFi_Send = false;
  bool fHaveSD_pre = fHaveSD;
  for (;;)
  {
    xWasDelayed = xTaskDelayUntil(&xLastWakeTime, do_WiFi_Send ? 10000 : 3000);
    if (!xWasDelayed)
      Debug.println("[Warning] Task Save Time Out.");
    // Check SD State and save Debug String every Loop
    // Create and Save Data to file if fSave = true;
    String Msg = "";
    String Data = "";
    byte isSDSave = sdCard.Err_SD_Off;
    fHaveSD_pre = fHaveSD;
    if (fSave && Calculate.State == Calculate.Done)
    {
      // Save Msg
      Msg += Clock.DateTimeStamp() + ",";
      Msg += rfid.ID + ",";
      Msg += String(Calculate.ResultAngle[0], 2) + ",";
      Msg += String(Calculate.ResultAngle[1], 2) + ",";
      Msg += String(Standard.Standard) + ",";
      Msg += String(imu.Gravity) + ",";
      Msg += String(imu.SensorTemperature) + "\n";
      isSDSave = sdCard.Save("/" + Clock.DateStamp("", 4), Msg);
      fSave = false;
      LED.Set(1, (isSDSave == sdCard.SDOK) ? LED.G : LED.Y, 2, 5, 3);
      if (isSDSave == sdCard.SDOK)
        Calculate.Switch(false);
      // Send Net
      Data += "&time=" + Clock.DateTimeStamp("_");
      Data += "&rfid=" + rfid.ID;
      Data += "&standard=" + String(Standard.Standard);
      Data += "&angle1=" + String(Calculate.ResultAngle[0], 4);
      Data += "&angle2=" + String(Calculate.ResultAngle[1], 4);
      Data += "&gravity=" + String(imu.Gravity % 3);
      Data += "&sensor_temperature=" + String(imu.SensorTemperature, 2);
      do_WiFi_Send = Net_Send_Measure(Data);
    }
    else
    {
      do_WiFi_Send = Net_Send_Measure("");
      isSDSave = sdCard.Save("", Msg);
    }
    // doRFID = (isSDSave == sdCard.SDOK || isSDSave == sdCard.Err_SD_Off);
    fHaveSD = (isSDSave == sdCard.SDOK || isSDSave == sdCard.Err_File_Write_Failed);
    if (fHaveSD != fHaveSD_pre)
    {
      if (fHaveSD) // Check the file when the sd card first detected.
      {
        String Info = sdCard.Read("/Setting.txt");
        if (Info.indexOf("Full_Cal_Password=123456789") != -1)
        {
          imu.ExpertMode = true;
          if (!TestVersion)
          {
            Debug.Setup(sdCard);
            Debug.printOnTop("========================Expert_Mode_On========================");
          }
        }
        Net_Set(Info);
      }
      else
      {
        imu.ExpertMode = false;
        imu.Cursor = 0;
        imu.CursorStart = 0;
      }
    }
    LED.Set(1, LED.Y, (fHaveSD) ? 0 : ((isSDSave == sdCard.Err_SD_Off) ? 30 : 3), 3);
  }
}

bool ButPress[3] = {false};

void ButtonChange0()
{
  if (digitalRead(Pin_Button0))
  { // Release
    Swich.Off_Clock_Stop();
    LED.Set(0, 0, 0, 4);
    ButPress[0] = true;
  }
  else
  { // Press
    Swich.Off_Clock_Start();
  }
}

void ButtonChange1()
{
  ButPress[1] = true;
}

void ButtonChange2()
{
  ButPress[2] = true;
}

void ButtonUpdate()
{
  if (millis() - LastEdit < FastLoopDelay * 2 || millis() < oled.BlockTime)
    memset(ButPress, false, sizeof(ButPress));
  if (ButPress[0] || ButPress[1] || ButPress[2])
    LastEdit = millis();
  else if (oled.Page != 0 && millis() - LastEdit > 10000 && !TestVersion && oled.Page != 8)
    oled.Page = 0;

  bool ButtonUp = (imu.GravityPrevious == 4) ? ButPress[2] : ButPress[1];
  bool ButtonDown = (imu.GravityPrevious == 4) ? ButPress[1] : ButPress[2];
  bool ButtonLeft = (imu.Gravity == 0) ? ButPress[1] : ButPress[2];
  bool ButtonRight = (imu.Gravity == 0) ? ButPress[2] : ButPress[1];
  bool ButtonAdd, ButtonMin;
  if (imu.Gravity == 0)
  {
    ButtonAdd = ButPress[2];
    ButtonMin = ButPress[1];
  }
  else if (imu.Gravity == 3)
  {
    ButtonAdd = ButPress[1];
    ButtonMin = ButPress[2];
  }
  else
  {
    ButtonAdd = ButtonDown;
    ButtonMin = ButtonUp;
  }
  switch (oled.Page)
  {
  case 0: // Main Page
    if (ButPress[0])
    {
      if (Calculate.State == Calculate.Not_Stable || Calculate.State == Calculate.Done || fSave)
      {
        Calculate.Switch(0);
        fSave = false;
      }
      else
      {
        oled.Page = 1;
        oled.MenuCursor = 0;
      }
    }
    else if (ButPress[1] && imu.fWarmUp == 100)
    {
      Calculate.Switch((Calculate.State != Calculate.Not_Stable) && (Calculate.State != Calculate.Done));
      fSave = false;
      // rfid.Reset();
    }
    else if (ButPress[2])
    {
      sdCard.Swich(1);
      if (imu.fWarmUp == 100)
      {
        if (Calculate.State == Calculate.Not_Stable)
        {
          Calculate.Switch(0);
          fSave = false;
        }
        else if (fSave)
        {
          Calculate.Switch(0);
          fSave = false;
        }
        else
        {
          fSave = true;
          if (Calculate.State != Calculate.Done)
            Calculate.Switch(1);
        }
      }
    }
    break;
  case 1: // Menu Select Page
    if (ButtonAdd)
      oled.MenuCursor++;
    if (ButtonMin)
      oled.MenuCursor += (imu.fWarmUp == 100) ? 7 : 6;
    oled.MenuCursor %= (imu.fWarmUp == 100) ? 8 : 7;
    if (ButPress[0])
    {
      if (oled.MenuCursor == 0)
        oled.Page = 0;
      else
        oled.Page = oled.MenuCursor + 1;
    }
    break;
  case 2:
    if (ButtonUp)
      Standard.Add(-1);
    if (ButtonDown)
      Standard.Add(1);
    if (ButPress[0])
      oled.Page = 0;
    break;
  case 3:
    if (ButPress[0])
      oled.Page = 0;
    if (ButtonUp)
      oled.ShowUnit = (oled.ShowUnit + 2) % 3;
    if (ButtonDown)
      oled.ShowUnit = (oled.ShowUnit + 1) % 3;
    break;
  case 4: // WiFi Page
    if (ButPress[0])
      oled.Page = 0;
    if (ButtonMin)
      WiFiChannel(-1);
    if (ButtonAdd)
      WiFiChannel(1);
    break;
  case 5: // SD Card Page
    if (ButtonUp && sdCard.Cursor > 0)
      sdCard.Cursor--;
    if (ButtonDown && sdCard.Cursor < sdCard.Show.endsWith(".csv") + 1)
      sdCard.Cursor++;
    if (ButPress[0] && sdCard.Cursor == 0)
      oled.Page = 0;
    if (ButPress[0] && sdCard.Cursor == 1)
      sdCard.Swich();
    if (ButPress[0] && sdCard.Cursor == 2)
      oled.EasyBlock(sdCard.Show);
    break;
  case 6: // Clock Page
    if (Clock.Cursor == -1)
    {
      if (ButPress[0])
        oled.Page = 0;
      else if (ButtonDown)
        Clock.ResetUserSetTimeBuffer();
      else if (ButtonUp)
      {
        Clock.ResetUserSetTimeBuffer();
        Clock.Cursor = 3;
      }
    }
    else
    {
      if (ButPress[0])
        Clock.UserSetTime(1);
      if (ButtonUp)
        Clock.UserSetTime(0);
      if (ButtonDown)
        Clock.UserSetTime(2);
      if (millis() - LastEdit > 3000)
      {
        Clock.Cursor = 6;
        Clock.UserSetTime(1);
        Clock.Cursor = -1;
        LED.Set(0, LED.M, 2, 3, 3);
      }
    }
    break;
  case 7: // Battery Page
    if (ButPress[0])
      oled.Page = 0;
    break;
  case 8: // Calibration Page
    if (imu.CalibrateCheck == -1 && imu.Cursor == 0 && ButPress[0])
      oled.Page = 1;
    else
    {
      if (ButPress[0])
        imu.CalibrateSelect(0);
      if (ButtonUp)
        imu.CalibrateSelect(1);
      if (ButtonDown)
        imu.CalibrateSelect(2);
      if (imu.CalibrateCheck == 0 && ButtonMin)
        imu.CalibrateSelect(3);
      if (imu.CalibrateCheck == 0 && ButtonAdd)
        imu.CalibrateSelect(4);
    }

    imu.Calibrate();
    if (imu.CalibrateCheck == 2)
    {
      oled.Block((imu.Cursor == 2) ? "Calibration Data Clear" : "Calibrate Complete");
      imu.CalStop();
      oled.Page = 0;
    }
    break;
  default:
    oled.Page = 0;
  }
  memset(ButPress, false, sizeof(ButPress));
}

void setup()
{
  LED.SetPin(0, Pin_LED_0_R, Pin_LED_0_G, Pin_LED_0_B);
  Bat.SetPin(Pin_Battery);
  Swich.On(Pin_Button_Wakeup, Pin_SwichEN, LED, Bat, oled);
  Serial.begin(115200);
  pinMode(IO_RFID_RST, OUTPUT);
  pinMode(IO_RFID_RST, HIGH);
  Wire.begin();
  Wire.setClock(400000);
  if (TestVersion)
    Debug.Setup(sdCard);

  Clock.Initialize(MainLoopDelay);
  if (Clock.RtcLostPower)
    LED.Set(0, LED.M, 1, 0);
  Debug.SetRTC(&Clock);
  Debug.printOnTop("-------------------------ESP_Start-------------------------");
  Debug.println("[Battery] Battery " + String(Bat.Percent) + " %");
  Swich.pSD = &sdCard;
  Swich.LastTriggure = &LastEdit;
  rfid.Initialize(IO_Buzzer);
  imu.Initialize(Pin_IMU_RX, Pin_IMU_TX);
  LED.SetPin(1, Pin_LED_1_R, Pin_LED_1_G, Pin_LED_1_B);
  oled.Initialize();
  Calculate.pLED = &LED;
  rfid.pLED = &LED;
  imu.pLED = &LED;
  imu.fWarmUpTime = &LastEdit;
  oled.ClockShow = &Clock;
  oled.Bat = &Bat.Percent;
  oled.ID = &rfid.ID;
  oled.imu = &imu;
  oled.Measure = &Calculate;
  oled.SDState = &fHaveSD;
  oled.fSave = &fSave;
  oled.pWifiState = &WiFiState;
  oled.Standard = &Standard;
  oled.pSD = &sdCard;
  while (!digitalRead(Pin_Button0))
    delay(1);
  LED.Set(0, 0, 0, 4);
  LED.Update();
  xTaskCreatePinnedToCore(FAST, "Core 1 Fast Loop", 10000, NULL, 3, T_FAST, 1);
  xTaskCreatePinnedToCore(MAIN, "Core 1 Main Loop", 16384, NULL, 2, T_MAIN, 1);
  xTaskCreatePinnedToCore(SLOW, "Core 0 Slow Loop", 10000, NULL, 1, T_SLOW, 0);
  xTaskCreatePinnedToCore(Save, "Core 0 Save Data", 10000, NULL, 2, T_SAVE, 0);
  delay(100);
  attachInterrupt(digitalPinToInterrupt(Pin_Button2), ButtonChange2, FALLING);
  attachInterrupt(digitalPinToInterrupt(Pin_Button1), ButtonChange1, FALLING);
  attachInterrupt(digitalPinToInterrupt(Pin_Button0), ButtonChange0, CHANGE);
  Serial.println("Setup Done");
}

void loop()
{
  // put your main code here, to run repeatedly:
}