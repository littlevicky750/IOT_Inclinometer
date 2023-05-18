#ifndef IMU42688_h
#define IMU42688_h
#include <Arduino.h>
#include "WC_IMU.h"
#include "LEDFlash.h"

#ifndef IMU_C
#define IMU_C 3
#endif

#ifndef TestVersion
#define TestVersion false
#endif

class IMU42688
{
private:
    float AngleMeasure[IMU_C][3];
    int IMUStart = 0;
    int CopeFailed = 0;
    int StartCalG;
    float StartCalA[3];
    float SumCalA[3];
    float FullCalAngle[6] = {0.0};
    float CalibrateCollect[6][10] = {0.0};
    //float CalCollectedAngle[3][100] = {0};
    //bool CalDevCount[100] = {1};
    //const int CalCount = 100;
    void CollectCalData();
    void QuickCalibrate();
    void FullCalibrate();
    void FullCalibrate_Z();
    float Avg_in_2StDev(float *Angle, bool *Count, int length);
    float SensorTemperatureCollect[2] = {0};
    float StartTemperature = 25;
    float WarmUpTemperature = 40;

public:
    float Angle[3] = {0, 0, 0};
    float AngleShow[3] = {0, 0, 0};
    float SensorTemperature;
    float e[3] = {0.0};
    float s[3] = {0.0};
    float b[3] = {0.0};
    byte Gravity = 2;
    byte GravityPrevious = 1;
    const byte IMU_Update_Success = 0;
    const byte Err_IMU_Not_Warm_Up = 1;
    const byte Err_IMU_Receive_Data_Error = 2;
    const byte Err_IMU_Data_StDev_Outside_TrustHold = 3;
    const byte Err_IMU_Cope_Failed = 4;
    byte ErrorCode = Err_IMU_Not_Warm_Up;
    byte fWarmUp;
    int *fWarmUpTime;
    LEDFlash *pLED;
    void Initialize(byte Rx /*(-1)*/, byte Tx /*(-1)*/);
    byte Update();
    float getHorizontal();
    float getVertical();
    float getHorizontalFilt();
    float getVerticalFilt();
    void Calibrate();
    void CalibrateSelect(byte Do);
    void CalStop();
    int CalibrateCheck = -1; // -1:Menu, 0:Select Yes No, 1:Collecting Data, 2: Finish
    int CalibrateCount = 0;
    int CalibrateCollectCount[6] = {0};
    byte CalAvgNum = 50;
    int Cursor = 0;
    int CursorStart = 0;
    int FullCalStep = 0;
    bool YesNo = false;
    bool FullCalComplete[6] = {false};
    bool ExpertMode = false;
};

#endif