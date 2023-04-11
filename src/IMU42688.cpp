#include "IMU42688.h"
#include <Preferences.h>

WC_IMU IMU;
Preferences pref;

void IMU42688::Initialize(byte Rx /*(-1)*/, byte Tx /*(-1)*/)
{
    Serial1.setRxBufferSize(256);
    Serial1.begin(921600, SERIAL_8N1, Rx, Tx);
    IMUStart = millis();
    memset(AngleMeasure, 0, sizeof(AngleMeasure));
    pref.begin("Angle_Cal", false);
    e[0] = pref.getFloat("Ex", 0.0);
    e[1] = pref.getFloat("Ey", 0.0);
    e[2] = pref.getFloat("Ez", 0.0);
    s[0] = pref.getFloat("Sx", 1.0);
    s[1] = pref.getFloat("Sy", 1.0);
    s[2] = pref.getFloat("Sz", 1.0);
    b[0] = pref.getFloat("Bx", 0.0);
    b[1] = pref.getFloat("By", 0.0);
    b[2] = pref.getFloat("Bz", 0.0);
    pref.end();
}

byte IMU42688::Update()
{
    // read multiple time in case of wrong coping
    for (int j = 0; j < IMU_C * 2; j++)
    {
        // Read Data from Serial1 --------------------------
        CopeFailed++;
        // Serial.print("Reading");
        int StartTime = millis();
        while (Serial1.available() && millis() - StartTime < 1000 && !IMU.newData)
        {
            // Serial.print(".");
            IMU.readSerialData(Serial1.read());
        }
        // Serial.println("");

        float AngleCope[6] = {0};
        byte Gravity_cope = 0;
        
        // Check if Got New Data -------------------------------------
        if (IMU.newData)
        {
            CopeFailed = 0;
            IMU.parseData();
        }
        else
        {
            ErrorCode = CopeFailed - 1 + Err_IMU_Cope_Failed;
            goto NextLoop;
        }

        // Read Angle and do basic check -----------------------------

        Gravity_cope = IMU.RecievedIMUData[7];
        if (Gravity_cope < 0 || Gravity_cope > 6)
        {
            ErrorCode = Err_IMU_Receive_Data_Error;
            goto NextLoop;
        }
        for (size_t i = 0; i < 6; i++)
        {
            AngleCope[i] = IMU.RecievedIMUData[i];
            if (AngleCope[i] == 0 || abs(AngleCope[i]) > 180)
            {
                ErrorCode = Err_IMU_Receive_Data_Error;
                goto NextLoop;
            }
        }
        memmove(&Angle[0], &AngleCope[0], sizeof(Angle));
        memmove(&AngleShow[0], &AngleCope[3], sizeof(Angle));
        Gravity = Gravity_cope;
        if (Gravity_cope == 4 || Gravity_cope == 1)
            GravityPrevious = Gravity_cope;
        SensorTemperature = IMU.RecievedIMUData[6];
        ErrorCode = IMU_Update_Success;
        /*
        Serial.print("X : ");
        Serial.print(AngleShow[0], 3);
        Serial.print(", Y : ");
        Serial.print(AngleShow[1], 3);
        Serial.print(", Z : ");
        Serial.print(AngleShow[2], 3);
        Serial.print(", g : ");
        Serial.print(", X : ");
        Serial.print(Angle[0], 3);
        Serial.print(", Y : ");
        Serial.print(Angle[1], 3);
        Serial.print(", Z : ");
        Serial.print(Angle[2], 3);
        Serial.print(", g : ");
        Serial.println(Gravity);
        //*/
        break;
    NextLoop:
        delay(10);
    } // end for

    // Check Warm Up Time ------------------------------
    if (fWarmUp != 100 && millis() - IMUStart < WarmUpTime)
    {
        ErrorCode = Err_IMU_Not_Warm_Up;
        fWarmUp = (millis() - IMUStart) * 100 / WarmUpTime;
        pLED->Set(0, pLED->Y, 1, 2);
    }
    else if (fWarmUp != 100)
    {
        fWarmUp = 100;
        pLED->Set(0, 0, 0, 2);
    }
    return ErrorCode;
} // end void Update()

float IMU42688::getHorizontal()
{
    float Ha = (Angle[1] + e[1]) * s[1];
    if (Gravity == 1 || Gravity == 4)
    {
        Ha = -(Angle[2] + e[2]) * s[2];
    }
    if (Ha > 180)
        Ha -= 360;
    else if (Ha < -180)
        Ha += 360;
    return Ha;
}

float IMU42688::getVertical()
{
    float Va = (Angle[0] + e[0]) * s[0];
    if (Gravity == 0 || Gravity == 3)
    {
        Va = 90 - (Angle[2] + e[2]) * s[2];
    }
    if (Va > 180)
        Va -= 360;
    else if (Va < -180)
        Va += 360;
    return Va;
}

float IMU42688::getHorizontalFilt()
{
    float Ha = (AngleShow[1] + e[1]) * s[1];
    if (Gravity == 1 || Gravity == 4)
    {
        Ha = -(AngleShow[2] + e[2]) * s[2];
    }
    if (Ha > 180)
        Ha -= 360;
    else if (Ha < -180)
        Ha += 360;
    return Ha;
}

float IMU42688::getVerticalFilt()
{
    float Va = (AngleShow[0] + e[0]) * s[0];
    if (Gravity == 0 || Gravity == 3)
    {
        Va = 90 - (AngleShow[2] + e[2]) * s[2];
    }
    if (Va > 180)
        Va -= 360;
    else if (Va < -180)
        Va += 360;
    return Va;
}

void IMU42688::CollectCalData()
{
    if (CalibrateCount == 0)
    {
        StartCalG = Gravity;
        memmove(&StartCalA[0], &Angle[0], sizeof(StartCalA));
    }
    else if (Gravity != StartCalG)
    {
        CalibrateCount = 0;
        memset(&SumCalA, 0, sizeof(SumCalA));
        return;
    }
    for (int i = 0; i < 3; i++)
    {
        if (i != StartCalG % 3 && abs(Angle[i] - StartCalA[i] > 0.5))
        {
            CalibrateCount = 0;
            memset(&SumCalA, 0, sizeof(SumCalA));
            return;
        }
        SumCalA[i] += Angle[i];
    }
    CalibrateCount++;
}

void IMU42688::QuickCalibrate()
{
    CollectCalData();
    if (CalibrateCount == 25)
    {
        pref.begin("Angle_Cal", false);
        switch (StartCalG)
        {
        case 0:
            e[2] = 90 - SumCalA[2] / 25;
            pref.putFloat("Ez", e[2]);
            break;
        case 1:
            e[2] = -SumCalA[2] / 25;
            pref.putFloat("Ez", e[2]);
            break;
        case 2:
            e[0] = -SumCalA[0] / 25;
            e[1] = -SumCalA[1] / 25;
            pref.putFloat("Ex", e[0]);
            pref.putFloat("Ey", e[1]);
            break;
        case 3:
            e[2] = -90 - SumCalA[2] / 25;
            pref.putFloat("Ez", e[2]);
            break;
        case 4:
            if (SumCalA[2] > 0)
                e[2] = 180 - SumCalA[2] / 25;
            else
                e[2] = -180 - SumCalA[2] / 25;
            ;
            pref.putFloat("Ez", e[2]);
            break;
        case 5:
            if (SumCalA[0] > 0)
                e[0] = 180 - SumCalA[0] / 25;
            else
                e[0] = -180 - SumCalA[0] / 25;
            if (SumCalA[1] > 0)
                e[1] = 180 - SumCalA[1] / 25;
            else
                e[1] = -180 - SumCalA[1] / 25;
            pref.putFloat("Ex", e[0]);
            pref.putFloat("Ey", e[1]);
            break;
        }
        pref.end();
        CalibrateCheck = 2;
    }
}

void IMU42688::FullCalibrate()
{
    if (Gravity > 2 || FullCalComplete[Gravity])
    {
        return;
    }

    CollectCalData();

    if (CalibrateCount == 25)
    {
        FullCalAngle[4 - StartCalG * 2] = SumCalA[(StartCalG + 1) % 3] / 25.0;
        FullCalAngle[5 - StartCalG * 2] = SumCalA[(StartCalG + 2) % 3] / 25.0;
        FullCalComplete[Gravity] = true;
        CalibrateCount = 0;
        CalibrateCheck = 0;
        memset(&SumCalA, 0, sizeof(SumCalA));
    }

    if (FullCalComplete[0] && FullCalComplete[1] && FullCalComplete[2])
    {
        pref.begin("Angle_Cal", false);
        char prefkey[9][3] = {"Sx","Sy","Sz","Bx","By","Bz","Ex","Ey","Ez"};
        for (int i = 0; i < 3; i++)
        {
            s[i] = 90.0 * pow(-1, i) / (FullCalAngle[i+3] - FullCalAngle[i]);
            b[i] = - FullCalAngle[i];
            e[i] = b[i];
            pref.putFloat(&prefkey[i][0],s[i]);
            pref.putFloat(&prefkey[i+3][0],b[i]);
            pref.putFloat(&prefkey[i+6][0],e[i]);
            Debug.print(String(prefkey[i])+" = "+String(s[i],6) + ", ");
            Debug.print(String(prefkey[i+3])+" = "+String(b[i],6)+ ", ");
            Debug.println(String(prefkey[i+6])+" = "+String(e[i],6));
        }
        pref.end();
        CalibrateCheck = 2;
    }
}

void IMU42688::Calibrate()
{
    if (CalibrateCheck != 1)
        return;
    if (Cursor == 1)
    {
        QuickCalibrate();
    }
    else if (Cursor == 2)
    {
        e[0] = b[0];
        e[1] = b[1];
        e[2] = b[2];
        pref.begin("Angle_Cal", false);
        pref.putFloat("Ex", b[0]);
        pref.putFloat("Ey", b[1]);
        pref.putFloat("Ez", b[2]);
        pref.end();
        CalibrateCheck = 2;
    }
    else if (Cursor == 3)
    {
        FullCalibrate();
    }
}

void IMU42688::CalStop()
{
    CalibrateCheck = -1;
    CalibrateCount = 0;
    memset(&SumCalA, 0, sizeof(SumCalA));
    memset(&FullCalComplete, false, sizeof(FullCalComplete));
    FullCalStep = 0;
    Cursor = 0;
    CursorStart = 0;
    YesNo = false;
}
