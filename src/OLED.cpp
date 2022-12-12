#include "OLED.h"
#include "OLED_XBM.h"
#define OLED1106

#ifdef OLED1309
U8G2_SSD1309_128X64_NONAME0_F_HW_I2C u8g2(U8G2_R0);
#else
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
#endif

#ifndef TestVersion
#define TestVersion false
#endif

void OLED::Begin()
{
    if (!isU8G2Begin)
    {
        Wire.begin();
        Wire.beginTransmission(60);
        byte error = Wire.endTransmission();
        if (error == 0)
            I2C_Add = 0x3C;
        else
        {
            Wire.beginTransmission(61);
            error = Wire.endTransmission();
            if (error == 0)
                I2C_Add = 0x3D;
        }

        if (I2C_Add == 0x00)
        {
            Debug.println("[OLED] OLED Address Not Found. Try to begin u8g2 anyway.");
        }
        else
        {
            u8g2.setI2CAddress(I2C_Add * 2);
            Debug.println("[OLED] OLED begin, Address = 0x" + String(I2C_Add, HEX));
        }
        while (millis() < 1000)
            ;
        u8g2.begin();
        isU8G2Begin = true;
        u8g2.clear();
        u8g2.sendBuffer();
    }
}

void OLED::Initialize()
{
    Begin();
    u8g2.enableUTF8Print();
    u8g2.setFlipMode(0);
    u8g2.setFont(Default_Font);
    u8g2.setFontDirection(0);
    u8g2.setFontMode(1);
    u8g2.setDrawColor(1);
    u8g2.clearBuffer();
    char S[19] = "IoT  Spirit  Level";
    u8g2.drawStr(64 - u8g2.getStrWidth(S) / 2, 30, S);
    char S1[17] = "V 3.1";
    u8g2.drawStr(64 - u8g2.getStrWidth(S1) / 2, 48, S1);
    u8g2.sendBuffer();
}

void OLED::Clear()
{
    Begin();
    u8g2.clearBuffer();
    u8g2.sendBuffer();
}

void OLED::ShowLowPower()
{
    Begin();
    u8g2.setFont(u8g2_font_open_iconic_check_2x_t);
    u8g2.setFontDirection(0);
    u8g2.setFontMode(1);
    u8g2.clearBuffer();
    u8g2.drawRBox(30, 17, 68, 35, 7);
    u8g2.drawRBox(89, 27, 15, 15, 7);
    u8g2.setDrawColor(0);
    u8g2.drawRBox(33, 20, 62, 29, 5);
    u8g2.setDrawColor(1);
    u8g2.drawGlyph(57, 42, 0x0044);
    u8g2.sendBuffer();
}

void OLED::TurnOff()
{
    Begin();
    u8g2.setPowerSave(1);
}

void OLED::Block(String BlockInfo)
{
    Begin();
    BlockTime = millis() + 3000;
    u8g2.clearBuffer();
    if (imu->GravityPrevious == 4)
        u8g2.setDisplayRotation(U8G2_R2);
    else
        u8g2.setDisplayRotation(U8G2_R0);
    u8g2.setFont(Default_Font);
    int w = u8g2.getStrWidth(BlockInfo.c_str());
    u8g2.drawStr(64 - w / 2, 40, BlockInfo.c_str());
    u8g2.sendBuffer();
}

void OLED::Update()
{
    FlashCount++;
    if (millis() < BlockTime)
    {
        return;
    }
    u8g2.clearBuffer();
    int Rotation = (imu->Gravity % 3 == 2) ? imu->GravityPrevious : imu->Gravity;
    switch (Page)
    {
    case 1:
        switch (Rotation)
        {
        case 0:
            u8g2.setDisplayRotation(U8G2_R1);
            Menu(0, 3, V);
            break;
        case 3:
            u8g2.setDisplayRotation(U8G2_R3);
            Menu(1, 0, V);
            break;
        case 4:
            u8g2.setDisplayRotation(U8G2_R2);
            Menu(3, 1, H);
            break;
        default:
            u8g2.setDisplayRotation(U8G2_R0);
            Menu(0, 0, H);
            break;
        }
        break;
    case 2:
        if (imu->GravityPrevious == 4)
            u8g2.setDisplayRotation(U8G2_R2);
        else
            u8g2.setDisplayRotation(U8G2_R0);
        Mode();
        break;
    case 3:
        if (imu->GravityPrevious == 4)
            u8g2.setDisplayRotation(U8G2_R2);
        else
            u8g2.setDisplayRotation(U8G2_R0);
        Unit();
        break;
    case 4:
        switch (Rotation)
        {
        case 0:
            u8g2.setDisplayRotation(U8G2_R1);
            Wifi(16, 52, 1);
            break;
        case 3:
            u8g2.setDisplayRotation(U8G2_R3);
            Wifi(16, 38, 3);
            break;
        case 4:
            u8g2.setDisplayRotation(U8G2_R2);
            Wifi(55, 13, 2);
            break;
        default:
            u8g2.setDisplayRotation(U8G2_R0);
            Wifi(41, 13, 0);
            break;
        }
        break;
    case 5:
        if (imu->GravityPrevious == 4)
            u8g2.setDisplayRotation(U8G2_R2);
        else
            u8g2.setDisplayRotation(U8G2_R0);
        Clock();
        Battery(97, 0, H);
        break;
    case 6:
        if (imu->GravityPrevious == 4)
        {
            u8g2.setDisplayRotation(U8G2_R2);
            Save(25, 0);
            u8g2.setFont(u8g2_font_open_iconic_embedded_2x_t);
            u8g2.drawGlyph(0, 56, 0x004e);
        }
        else
        {
            u8g2.setDisplayRotation(U8G2_R0);
            Save(0, 0);
            u8g2.setFont(u8g2_font_open_iconic_embedded_2x_t);
            u8g2.drawGlyph(114, 55, 0x004e);
        }
        break;
    case 7:
        if (Rotation == 0)
            u8g2.setDisplayRotation(U8G2_R1);
        else if (Rotation == 3)
            u8g2.setDisplayRotation(U8G2_R3);
        else if (Rotation == 4)
            u8g2.setDisplayRotation(U8G2_R2);
        else
            u8g2.setDisplayRotation(U8G2_R0);
        Battery(u8g2.getDisplayWidth()/2 - 32, u8g2.getDisplayHeight()/2 - 24, 64, 26, 6);
        char B[6];
        sprintf(B,"%d %%",min(max(*Bat,0),100));
        u8g2.setFont(Default_Font);
        u8g2.drawStr((u8g2.getDisplayWidth() - u8g2.getStrWidth(B)) / 2, u8g2.getDisplayHeight()/2 + 24, B);
        break;
    default:
        CheckState();
        if (Rotation == 0)
            u8g2.setDisplayRotation(U8G2_R1);
        else if (Rotation == 3)
            u8g2.setDisplayRotation(U8G2_R3);
        else if (Rotation == 4)
            u8g2.setDisplayRotation(U8G2_R2);
        else
            u8g2.setDisplayRotation(U8G2_R0);
        if (Rotation % 3 == 0)
        {
            Battery(0, 122, 28, 6, 2);
            ICON(0, 93, 28, 28);
            QuickInfo(29, 93, 35, 35);
            Describe(0, 0, 63, 18, V);
            Main2(0, 21, 64, 70);
        }
        else
        {
            Main2(44, 22, 84, 42);
            Describe(40, 0, 87, 18, H);
            QuickInfo(0, 32, 41, 32);
            ICON(11, 0, 30, 30);
            Battery(0, 0, 9, 30, 2);
        }
        break;
    }
    u8g2.sendBuffer();
}

void OLED::Unit()
{
    u8g2.setFont(Default_Font);
    u8g2.drawStr(0, 11, "Unit");
    u8g2.drawBox(0, 13, 128, 2);
    u8g2.drawBox(0, 17 + 16 * ShowUnit, 128, 16);
    u8g2.setDrawColor(2);
    char U[2][7] = {"degree", "radian"};
    u8g2.drawStr(64 - u8g2.getStrWidth(U[0]) / 2, 29, U[0]);
    u8g2.drawStr(64 - u8g2.getStrWidth(U[1]) / 2, 46, U[1]);
    u8g2.setDrawColor(1);
}

void OLED::Menu(int x, int y, bool isH)
{
    int PageNum = 7;
    int nx, ny;
    char TempTitle[PageNum][5] = {"HOME", "MODE", "UNIT", "WIFI", "Time", "SAVE", "BATT"};
    nx = 2 + 31 * ((isH) ? MenuCursor / 2 : MenuCursor % 2);
    ny = 2 + 31 * ((isH) ? MenuCursor % 2 : MenuCursor / 2);
    u8g2.drawRBox(x + nx, y + ny, 28, 28, 3);
    for (int i = 0; i < PageNum; i++)
    {
        u8g2.setDrawColor(1);
        nx = 31 * ((isH) ? i / 2 : i % 2);
        ny = 31 * ((isH) ? i % 2 : i / 2);
        u8g2.drawRFrame(x + nx, y + ny, 32, 32, 5);
        u8g2.setDrawColor(i != MenuCursor);
        if (i == 6)
        {
            Battery(x + nx + 6, y + ny + 11, H);
        }
        else if (i == 3)
        {
            u8g2.drawXBM(x + nx + 4, y + ny + 4, 24, 24, WiFi24x24[WiFiShow()]);
            if (pWifiState->now == pWifiState->Off)
            {
                u8g2.setFont(u8g2_font_6x13_tr);
                u8g2.drawStr(x + nx + 22, y + ny + 28, "X");
            }
            else
            {
                u8g2.setFont(Describe_Font);
                u8g2.drawGlyph(x + nx + 22, y + ny + 28, pWifiState->Channel + 48);
            }
        }
        else if (i == 0 || i == 4 || i == 5)
        {
            u8g2.drawXBM(x + nx + 4, y + ny + 4, 24, 24, Menu_XBM[i]);
        }
        else
        {
            u8g2.setFont(u8g2_font_bytesize_tr);
            for (int j = 0; j < 4; j++)
            {
                char A[2] = {TempTitle[i][j]};
                int w = u8g2.getStrWidth(A);
                u8g2.drawGlyph(x + nx + 10 + 12 * (j % 2) - w / 2, y + ny + 15 + 14 * (j / 2), TempTitle[i][j]);
            }
        }
    }
    u8g2.setDrawColor(1);
}

void OLED::Describe(int x, int y, int w, int h, bool isH)
{
    char S[8];
    if (State == 0)
        strcpy(S, "Warm Up");
    else if (State == 1)
        strcpy(S, "Ready");
    else if (State == 2)
        strcpy(S, "Hold");
    else if (State == 3)
        strcpy(S, "Hold");
    else if (State == 4)
        strcpy(S, "Done");
    else if (State == 5)
        strcpy(S, "Saving");
    else if (State == 6)
        strcpy(S, "No SD");
    else if (State == 7)
        strcpy(S, "Error");
    else
        strcpy(S, "Unknown");

    int ha, hb, ws, wb;
    wb = w / 10;
    if (true) //(isH)
    {
        u8g2.setFont(Default_Font);
        ha = 11;
        hb = 15;
        ws = u8g2.getStrWidth(S);
        u8g2.drawStr(x + w / 2 - ws / 2, y + ha, S);
    }
    else
    {
        u8g2.setFont(Describe_Font);
        ha = 8;
        hb = 22;
        ws = u8g2.getStrWidth(S);
        u8g2.drawStr(x + w / 2 - ws / 2, y + ha, S);
    }
    u8g2.drawHLine(x, y + h, w);
    if (h > hb + 1)
    {
        Count += 3;
        Count %= (w - wb) * 2;
        if (State == 0)
        {
            int L = max(min((imu->fWarmUp * w) / 100, w), 0);
            u8g2.drawBox(x, y + hb, L, h - hb);
        }
        else if (State == 3)
        {
            int L = max(min(Measure->MeasureCount * w / 10, w), 0);
            u8g2.drawBox(x, y + hb, L, h - hb);
        }
        else if (State == 4)
        {
            u8g2.drawBox(x, y + hb, w, h - hb);
        }
        else if (State == 2 || State == 5)
        {
            if (Count < w - wb)
                u8g2.drawBox(x + Count, y + hb, wb, h - hb);
            else
                u8g2.drawBox(x + 2 * w - 2 * wb - Count, y + hb, wb, h - hb);
        }
        u8g2.setDrawColor(0);
        u8g2.drawTriangle(x + w - (h - hb) + 1, y + hb, x + w, y + hb, x + w, y + h - 1);
        u8g2.setDrawColor(1);
        u8g2.drawHLine(x, y + hb, w - (h - hb));
        u8g2.drawLine(x + w - (h - hb), y + hb, x + w, y + h);
        u8g2.drawVLine(x, y + hb, h - hb);
    }
}

void OLED::Battery(int x, int y, int w, int h, int b)
{

    int B = (h > 20 && w > 20) ? 2 : 1;
    int d = (h < 8 || w < 8) ? 0 : B;

    if (h > w)
    {
        u8g2.drawBox(x + (w + 1) / 4, y, w - (w + 1) / 4 * 2, b);
        for (int i = 0; i < B; i++)
        {
            u8g2.drawFrame(x + i, y + b + i, w - 2 * i, h - b - 2 * i);
        }
        int LF = min(max(*Bat * (h - b - 2 * B - 2 * d) / 100, 0), (h - b - 2 * B - 2 * d));
        u8g2.drawBox(x + B + d, y + h - LF - B - d, w - 2 * B - 2 * d, LF);
    }
    else
    {
        u8g2.drawBox(x + w - b, y + (h + 1) / 4, b, h - (h + 1) / 4 * 2);
        for (int i = 0; i < B; i++)
        {
            u8g2.drawFrame(x + i, y + i, w - b - 2 * i, h - 2 * i);
        }
        int LF = min(max(*Bat * (w - b - 2 * B - 2 * d) / 100, 0), (w - b - 2 * B - 2 * d));
        u8g2.drawBox(x + B + d, y + B + d, LF, h - 2 * B - 2 * d);
    }
}

void OLED::QuickInfo(int x, int y, int w, int h)
{
    u8g2.drawRBox(x, y, w, (h - 1) / 2, 5);
    u8g2.drawRBox(x, y + h / 2 + 1, w, (h - 1) / 2, 5);
    u8g2.setFont(Describe_Font);
    u8g2.setDrawColor(0);
    char S1[5];
    char S2[5];
    sprintf(S1, "MD=%d", Standard->Standard);
    strcpy(S2, (ShowUnit == 0) ? "deg" : "rad");
    int hs = u8g2.getAscent() - u8g2.getDescent();
    int ws = u8g2.getStrWidth(S1);
    u8g2.drawStr(x + w / 2 - ws / 2, y + (h + 2 * hs) / 4 - 1, S1);
    ws = u8g2.getStrWidth(S2);
    u8g2.drawStr(x + w / 2 - ws / 2, y + (3 * h + 2 * hs) / 4 - 1, S2);
    u8g2.setDrawColor(1);
}

void OLED::Main2(int x, int y, int TW, int TH)
{
    char Title[3][3] = {"ID", "A1", "A2"};
    float An[2];
    int showdigit = 2;
    if (Measure->State == Measure->Done)
    {
        An[0] = Measure->ResultAngle[0];
        An[1] = Measure->ResultAngle[1];
    }
    else
    {
        An[0] = imu->getHorizontalFilt();
        An[1] = imu->getVerticalFilt();
    }
    if (ShowUnit == 1)
    {
        An[0] = An[0] * PI / 180.0;
        An[1] = An[0] * PI / 180.0;
        showdigit = 4;
    }

    char A[3][8];
    strcpy(A[0], ID->c_str());
    dtostrf(An[0], 7, showdigit, A[1]);
    dtostrf(An[1], 7, showdigit, A[2]);

    u8g2.setFont(MSpace_Font);
    int Sh = 10;
    int Dh, LH, S1h, S2h;
    if (TW > 64)
    {
        S1h = Sh;
        Dh = (TH - Sh * 3) / 4;
        S2h = Sh;
        LH = Sh + (TH - Sh * 3) / 2;
    }
    else
    {
        S1h = Sh;
        Dh = 2;
        S2h = (TH - Dh * 4 - 2) / 3;
        LH = S2h + Dh * 2 + 1;
    }
    for (int i = 0; i < 3; i++)
    {
        u8g2.drawStr(x + 4, y + S1h + LH * i, Title[i]);
        u8g2.drawStr(x + TW - u8g2.getStrWidth(A[i]) - 4, y + S2h + LH * i, A[i]);
        if (i != 2)
        {
            u8g2.drawBox(x, y + S2h + LH * i + Dh - 1, 3, 3);
            u8g2.drawBox(x + TW - 3, y + S2h + LH * i + Dh - 1, 3, 3);
            u8g2.drawHLine(x, y + S2h + LH * i + Dh, TW);
        }
    }
}

void OLED::ICON(int x, int y, int w, int h)
{
    u8g2.drawRBox(x, y, w, h, 5);
    u8g2.setDrawColor(0);
    if (State == 0)
        u8g2.drawXBM(x + w / 2 - 12, y + h / 2 - 12, 24, 24, Fire24x24);
    else if (State == 1)
        u8g2.drawXBM(x + w / 2 - 12, y + h / 2 - 12, 24, 24, Ready24x24);
    else if (State == 2 || State == 3)
    {
        if (Flash(6, 12))
            u8g2.drawXBM(x + w / 2 - 12, y + h / 2 - 12, 24, 24, Timer0_24x24);
        else
            u8g2.drawXBM(x + w / 2 - 12, y + h / 2 - 12, 24, 24, Timer2_24x24);
    }
    else if (State == 4)
        u8g2.drawXBM(x + w / 2 - 12, y + h / 2 - 12, 24, 24, Done24x24);
    else if (State == 5)
        u8g2.drawXBM(x + w / 2 - 12, y + h / 2 - 12, 24, 24, SDSave24x24);
    else if (State == 6)
        u8g2.drawXBM(x + w / 2 - 12, y + h / 2 - 12, 24, 24, NOSD24x24);
    u8g2.setDrawColor(1);
}

void OLED::CheckState()
{
    if (imu->fWarmUp != 100)
        State = 0;
    else if (Measure->State == Measure->Not_Stable)
        State = 2;
    else if (Measure->State == Measure->Measuring)
        State = 3;
    else if (*SDState == false)
        State = 6;
    else if (Measure->State == Measure->Sleep)
        State = 1;
    else if (*fSave == true)
        State = 5;
    else if (Measure->State == Measure->Done)
        State = 4;
    else
        State = 7;
}

void OLED::Clock()
{
    if (ClockShow->RtcLostPower)
    {
        u8g2.setFont(u8g2_font_open_iconic_embedded_2x_t);
        u8g2.drawGlyph(0, 41, 0x0047);
    }
    String NowSet = (ClockShow->Cursor == -1) ? "Now" : "Set";
    u8g2.setFont(Default_Font);
    u8g2.drawStr(9, 11, ClockShow->DateStamp(NowSet, "WWWWWW").c_str());
    if (ClockShow->Cursor != 0 || Flash(3, 6))
    {
        u8g2.drawStr(9, 64, ClockShow->DateStamp(NowSet, "YYYY").c_str());
    }
    if (ClockShow->Cursor != 1 || Flash(3, 6))
    {
        char S[4];
        strcpy(S, ClockShow->DateStamp(NowSet, "MMM").c_str());
        int w = u8g2.getStrWidth(S);
        u8g2.drawStr(72 - w / 2, 64, S);
    }
    if (ClockShow->Cursor != 2 || Flash(3, 6))
    {
        u8g2.drawStr(103, 64, ClockShow->DateStamp(NowSet, "DD").c_str());
    }
    u8g2.setFont(u8g2_font_helvR24_tn);
    String Time = ClockShow->TimeStamp(NowSet, "");
    if (ClockShow->Cursor != 3 || Flash(3, 6))
    {
        u8g2.drawGlyph(17, 45, Time.charAt(0));
        u8g2.drawGlyph(37, 45, Time.charAt(1));
    }
    if (ClockShow->Cursor != 4 || Flash(3, 6))
    {
        u8g2.drawGlyph(74, 45, Time.charAt(2));
        u8g2.drawGlyph(94, 45, Time.charAt(3));
    }
    if (ClockShow->Cursor != -1 || atoi(Time.substring(5).c_str()) % 2 != 1)
    {
        u8g2.drawGlyph(58, 42, 0x003a);
    }
}

void OLED::Save(int x, int y)
{
    u8g2.setFont(Default_Font);

    if (pSD->Show.endsWith(".csv"))
    {
        u8g2.drawStr(0, 11, "Ready");
        // u8g2.setFont(Describe_Font);
        u8g2.drawFrame(x, 22, 14, 10);
        u8g2.drawHLine(x, 21, 5);
        u8g2.drawHLine(x, 20, 4);
        u8g2.drawStr(x + 15, 31, ":");
        String path = pSD->Show.substring(1);
        int i = 0;
        int j = 0;
        while (j != -1)
        {
            j = path.indexOf("/");
            u8g2.drawStr(x + 21, 32 + i * 15, "/");
            u8g2.drawStr(x + 27, 32 + i * 15, path.substring(0, j).c_str());
            path = path.substring(j + 1);
            i++;
        }
    }
    else
    {
        u8g2.drawStr(0, 20, pSD->Show.c_str());
    }
}

void OLED::Mode()
{
    u8g2.setFont(Default_Font);
    u8g2.drawStr(0, 11, "Mode");
    u8g2.drawBox(0, 13, 128, 2);
    u8g2.drawBox(0, 17 + 16 * (Standard->Standard - Standard->flag), 128, 15);
    u8g2.setDrawColor(2);
    for (int i = 0; i < 3; i++)
    {
        u8g2.drawGlyph(60, 30 + 16 * i, Standard->flag + i + 48);
    }
    u8g2.setDrawColor(1);
}

void OLED::Battery(int x, int y, bool isH)
{
    int BatBox = min(max(*Bat / 7, 0), 14);
    u8g2.drawFrame(x, y, 18, 10);
    u8g2.drawBox(x + 18, y + 3, 2, 4);
    u8g2.drawBox(x + 2, y + 2, BatBox, 6);
}

int OLED::WiFiShow()
{
    if (pWifiState->now == 0)
        return 0;
    else if (pWifiState->now == 4)
    {
        int S = pWifiState->Signal;
        S = min(max(S, 1), 4);
        return S;
    }
    else if (Flash(5, 20))
        return 1;
    else if (Flash(10, 20))
        return 2;
    else if (Flash(15, 20))
        return 3;
    else
        return 4;
}

void OLED::Wifi(int x, int y, int R)
{
    // Button
    // u8g2.setFont(u8g2_font_victoriabold8_8r);

    u8g2.setFont(MSpace_Font);
    if (pWifiState->Channel < WiFi_Select_Flag)
        WiFi_Select_Flag = pWifiState->Channel;
    else if (pWifiState->Channel > WiFi_Select_Flag + 3)
        WiFi_Select_Flag = pWifiState->Channel - 3;
    if (R % 2 == 0)
    {
        int xb = (R == 0) ? 114 : 0;
        if (WiFi_Select_Flag == 0)
            u8g2.drawBox(xb + 2, 1, 10, 2);
        else
            u8g2.drawXBM(xb + 2, 0, 10, 3, Up10x3);
        if (WiFi_Select_Flag == 2)
            u8g2.drawBox(xb + 2, 61, 10, 2);
        else
            u8g2.drawXBM(xb + 2, 61, 10, 3, Down10x3);
        u8g2.drawRBox(xb, 4 + 14 * (pWifiState->Channel - WiFi_Select_Flag), 14, 14, 3);
        u8g2.setDrawColor(2);
        for (int i = 0; i < 4; i++)
        {
            if (i + WiFi_Select_Flag == 0)
                u8g2.drawStr(xb + 4, 16 + 14 * i, "X");
            else
                u8g2.drawGlyph(xb + 4, 16 + 14 * i, 48 + i + WiFi_Select_Flag);
        }
        u8g2.setDrawColor(1);
    }
    else
    {
        int yb = (R == 1) ? 0 : 114;
        if (WiFi_Select_Flag == 0)
            u8g2.drawBox(1, yb + 2, 2, 10);
        else
            u8g2.drawXBM(0, yb + 2, 3, 10, Left3x10);
        if (WiFi_Select_Flag == 2)
            u8g2.drawBox(61, yb + 2, 2, 10);
        else
            u8g2.drawXBM(61, yb + 2, 3, 10, Right3x10);
        u8g2.drawRBox(4 + 14 * (pWifiState->Channel - WiFi_Select_Flag), yb, 14, 14, 3);
        u8g2.setDrawColor(2);
        for (int i = 0; i < 4; i++)
        {
            if (i + WiFi_Select_Flag == 0)
                u8g2.drawStr(8 + 14 * i, yb + 12, "X");
            else
                u8g2.drawGlyph(8 + 14 * i, yb + 12, 48 + i + WiFi_Select_Flag);
        }
        u8g2.setDrawColor(1);
    }

    // Description
    u8g2.drawXBM(x, y, 31, 25, WiFi31x25[WiFiShow()]);
    u8g2.setFont(Describe_Font);
    char A[13];
    if (pWifiState->now == pWifiState->Off)
        strcpy(A, "Function Off");
    else if (pWifiState->now == pWifiState->On)
        strcpy(A, "No Signal");
    else if (pWifiState->now == pWifiState->ConnectingWifi)
        strcpy(A, "Connecting.");
    else if (pWifiState->now == pWifiState->ConnectingMqtt)
        strcpy(A, "Connecting..");
    else if (pWifiState->now == pWifiState->Finish)
        strcpy(A, "Connected");
    int w = u8g2.getStrWidth(A);
    u8g2.drawStr(x + 16 - w / 2, y + 38, A);
}

bool OLED::Flash(int Due, int Period)
{
    return (FlashCount % Period < Due);
}
