#include "OLED.h"
#include "OLED_XBM.h"
#define OLED1309

#ifndef IO_OLED_RST
#define IO_OLED_RST 19
#endif

#ifdef OLED1309
U8G2_SSD1309_128X64_NONAME0_F_HW_I2C u8g2(U8G2_R0, IO_OLED_RST);
#else
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
#endif

void OLED::Begin()
{
    if (!isU8G2Begin)
    {
        // Vdd On
        pinMode(IO_OLED_RST, OUTPUT);
        digitalWrite(IO_OLED_RST, LOW);
        delay(1);
        digitalWrite(IO_OLED_RST, HIGH);
        delay(1);
        // Vcc On
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
    char S[19] = "IoT  Inclinometer";
    u8g2.drawStr(64 - u8g2.getStrWidth(S) / 2, 30, S);
    char S1[17] = "V 3.10";
    u8g2.drawStr(64 - u8g2.getStrWidth(S1) / 2, 48, S1);
    u8g2.sendBuffer();
}

void OLED::Clear()
{
    Begin();
    u8g2.clear();
    u8g2.initDisplay();
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
    u8g2.clear();
    delay(1);
    u8g2.setPowerSave(1);
}

void OLED::Block(String BlockInfo)
{
    Begin();
    if (BlockInfo == "")
    {
        BlockTime = millis();
        return;
    }
    else
    {
        BlockTime = millis() + 3000;
        u8g2.clearBuffer();
        int Rotation = (imu->Gravity % 3 == 2) ? imu->GravityPrevious : imu->Gravity;
        switch (Rotation)
        {
        case 0:
            u8g2.setDisplayRotation(U8G2_R1);
            break;
        case 3:
            u8g2.setDisplayRotation(U8G2_R3);
            break;
        case 4:
            u8g2.setDisplayRotation(U8G2_R2);
            break;
        default:
            u8g2.setDisplayRotation(U8G2_R0);
            break;
        }
        u8g2.setFont(Default_Font);
        const unsigned int Mx = u8g2.getDisplayWidth();
        const unsigned int My = u8g2.getDisplayHeight();
        const unsigned int a = u8g2.getAscent() - u8g2.getDescent();
        int s[10] = {0};
        int s_now = BlockInfo.indexOf(" ");
        int s_next, w;
        int i = 0;
        if (s_now == -1)
        {
            s_now = BlockInfo.length();
            i++;
            s[i] = s_now + 1;
        }
        while (s_now < BlockInfo.length())
        {
            w = u8g2.getStrWidth(BlockInfo.substring(s[i], s_now).c_str());
            s_next = -1;
            while (w <= Mx)
            {
                s_now += s_next + 1;
                if (s_now > BlockInfo.length())
                    break;
                s_next = BlockInfo.substring(s_now + 1).indexOf(" ");
                if (s_next == -1)
                    s_next = BlockInfo.substring(s_now + 1).length();
                w = u8g2.getStrWidth(BlockInfo.substring(s[i], s_now + s_next + 1).c_str());
            }
            i++;
            s[i] = s_now + 1;
            s_next = BlockInfo.substring(s[i]).indexOf(" ");
            if (s_next == -1 && s_now < BlockInfo.length())
            {
                i++;
                s[i] = BlockInfo.length() + 1;
                break;
            }
            s_now += s_next + 1;
        }
        unsigned int sh = a * 1.25;
        unsigned int h = sh * (i - 1) + a;
        while (h > My)
        {
            sh--;
            h = sh * (i - 1) + a;
        }
        unsigned int y = (My - h) / 2 + u8g2.getAscent();
        for (int j = 0; j < i; j++)
        {
            w = u8g2.getStrWidth(BlockInfo.substring(s[j], s[j + 1] - 1).c_str());
            u8g2.drawStr((Mx - w) / 2, y, BlockInfo.substring(s[j], s[j + 1] - 1).c_str());
            y += sh;
        }
        u8g2.sendBuffer();
    }
}

void OLED::Update()
{
    FlashCount++;
    if (millis() < BlockTime)
        return;

    u8g2.clearBuffer();
    int Rotation_previous = Rotation;
    if (imu->Gravity % 3 == 2 || Page == 2 || Page == 3 || Page == 5 || Page == 6 || (Page == 8 && imu->CalibrateCheck == -1))
        Rotation = imu->GravityPrevious;
    else
        Rotation = imu->Gravity;

    if (Rotation != Rotation_previous)
    {
        if (Rotation == 0)
            u8g2.setDisplayRotation(U8G2_R1);
        else if (Rotation == 3)
            u8g2.setDisplayRotation(U8G2_R3);
        else if (Rotation == 4)
            u8g2.setDisplayRotation(U8G2_R2);
        else
            u8g2.setDisplayRotation(U8G2_R0);
    }

    switch (Page)
    {
    case 1:
        Menu();
        break;
    case 2:
        Mode();
        break;
    case 3:
        Unit();
        break;
    case 4:
        Wifi();
        break;
    case 5:
        Save();
        break;
    case 6:
        Clock();
        break;
    case 7:
        Battery(u8g2.getDisplayWidth() / 2 - 32, u8g2.getDisplayHeight() / 2 - 24, 64, 26, 6);
        char B[6];
        sprintf(B, "%d %%", min(max(*Bat, 0), 100));
        u8g2.setFont(Default_Font);
        u8g2.drawStr((u8g2.getDisplayWidth() - u8g2.getStrWidth(B)) / 2, u8g2.getDisplayHeight() / 2 + 24, B);
        break;
    case 8:
        if (imu->CalibrateCheck == -1)
            Cal_M();
        else
            Cal();
        break;
    default:
        CheckState();
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
    u8g2.drawBox(0, 13, 128, 1);
    char U[3][7] = {"degree", "mm  m", "radian"};
    int y[3] = {15, 33, 49};
    int dh[3] = {1, 2, 2};
    u8g2.drawBox(0, y[ShowUnit], 128, 17 - dh[ShowUnit]);
    u8g2.setDrawColor(2);
    u8g2.drawStr(64 - u8g2.getStrWidth(U[0]) / 2, y[0] + dh[0] + 11, U[0]);
    u8g2.drawStr(64 - u8g2.getStrWidth(U[1]) / 2, y[1] + dh[1] + 10, U[1]);
    u8g2.drawStr(64 - u8g2.getStrWidth(U[2]) / 2, y[2] + dh[2] + 11, U[2]);
    u8g2.drawStr(64 - u8g2.getStrWidth(U[2]) / 2 + u8g2.getStrWidth("mm") + 2, y[1] + dh[1] + 11, "/");
    u8g2.setDrawColor(1);
}

void OLED::Mode()
{
    u8g2.setFont(Default_Font);
    u8g2.drawStr(0, 11, "Mode");
    u8g2.drawBox(0, 13, 128, 2);
    u8g2.drawBox(0, 17 + 16 * (Standard->Standard - Standard->flag), 128, 15);
    u8g2.setDrawColor(2);
    for (int i = 0; i < 3; i++)
        u8g2.drawGlyph(60, 30 + 16 * i, Standard->flag + i + 48);
    u8g2.setDrawColor(1);
}

void OLED::Menu()
{
    int dx[5] = {0, 0, 0, 1, 3};
    int dy[5] = {3, 0, 0, 0, 1};
    int x = dx[Rotation];
    int y = dy[Rotation];
    bool isH = (Rotation % 3 != 0);
    int PageNum = (imu->fWarmUp == 100) ? 8 : 7;
    int nx, ny;
    char TempTitle[PageNum][5] = {"HOME", "MODE", "UNIT", "WIFI", "Time", "SAVE", "BATT", "CALI"};
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
        switch (i)
        {
        case 0:
            u8g2.drawXBM(x + nx + 4, y + ny + 4, 24, 24, Home24x24);
            break;
        case 2:
            u8g2.drawXBM(x + nx + 4, y + ny + 4, 24, 24, Unit24x24[ShowUnit]);
            break;
        case 3:
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
            break;
        case 4:
            if (pSD->Show.indexOf("OFF") != -1)
                u8g2.drawXBM(x + nx + 4, y + ny + 4, 24, 24, SDOFF24x24);
            else if (*SDState == false)
                u8g2.drawXBM(x + nx + 4, y + ny + 4, 24, 24, NOSD24x24);
            else if (*fSave == true)
                u8g2.drawXBM(x + nx + 4, y + ny + 4, 24, 24, SDSave24x24);
            else
                u8g2.drawXBM(x + nx + 4, y + ny + 4, 24, 24, SD24x24);
            break;
        case 5:
        {
            u8g2.drawXBM(x + nx + 4, y + ny + 4, 24, 24, BlankClock24x24);
            int num = ClockShow->now.minute();
            for (int l = 8; l > 4; l -= 3)
            {
                int px = x + nx + 15 + (num > 30);
                int py = y + ny + 16 - (num + 16) / 30 % 2;
                u8g2.drawLine(px, py, px + l * sin(num / 30.0 * PI) + 0.5, py - (l + 1) * cos(num / 30.0 * PI) + 0.5);
                num = ClockShow->now.twelveHour() * 5 + ClockShow->now.minute() / 12;
            }
            break;
        }
        case 6:
            Battery(x + nx + 5, y + ny + 10, 22, 12, 2);
            break;
        case 7:
            u8g2.drawXBM(x + nx + 4, y + ny + 4, 24, 24, Cal24x24);
            break;
        default:
            u8g2.setFont(u8g2_font_bytesize_tr);
            for (int j = 0; j < 4; j++)
            {
                char A[2] = {TempTitle[i][j]};
                int w = u8g2.getStrWidth(A);
                u8g2.drawGlyph(x + nx + 10 + 12 * (j % 2) - w / 2, y + ny + 15 + 14 * (j / 2), TempTitle[i][j]);
            }
            break;
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
    char S2[3][5] = {"deg", "mm/m", "rad"};
    sprintf(S1, "MD=%d", Standard->Standard);
    int hs = u8g2.getAscent() - u8g2.getDescent();
    int ws = u8g2.getStrWidth(S1);
    u8g2.drawStr(x + w / 2 - ws / 2, y + (h + 2 * hs) / 4 - 1, S1);
    ws = u8g2.getStrWidth(S2[ShowUnit]);
    u8g2.drawStr(x + w / 2 - ws / 2, y + (3 * h + 2 * hs) / 4 - 1, S2[ShowUnit]);
    u8g2.setDrawColor(1);
}

void OLED::Main2(int x, int y, int TW, int TH)
{
    char Title[3][3] = {"ID", "A1", "A2"};
    float An[2];
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
    char A[3][8];
    strcpy(A[0], ID->c_str());
    switch (ShowUnit)
    {
    case 1:
        for (int i = 0; i < 2; i++)
        {
            if (abs(An[i]) < 5.7 || abs(An[i]) > 174.3)
            {
                An[i] = round(tan(An[i] * PI / 180.0) * 5000) / 5.0;
                dtostrf(An[i], 7, 1, A[i + 1]);
            }
            else
                strcpy(A[i + 1], "-------");
        }
        break;
    case 2:
        dtostrf(An[0] * PI / 180.0, 7, 4, A[1]);
        dtostrf(An[1] * PI / 180.0, 7, 4, A[2]);
        break;
    default:
        dtostrf(An[0], 7, 2, A[1]);
        dtostrf(An[1], 7, 2, A[2]);
        break;
    }

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
    else if (*SDState == false && pSD->Show.indexOf("OFF") == -1)
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
    u8g2.drawStr(9, 13, ClockShow->DateStamp(NowSet, "WWW").c_str());
    String GMT = "GMT+" + String(pWifiState->GMT);
    int w = u8g2.getStrWidth(GMT.c_str());
    u8g2.drawStr(118 - w, 13, GMT.c_str());
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

void OLED::EasyBlock(String BlockInfo)
{
    BlockTime = millis() + 3000;
    u8g2.clearBuffer();
    String str = BlockInfo.substring(0, 1);
    String RemainString = BlockInfo.substring(1);

    int i = 0;
    int l = 0;
    while (l != -1)
    {
        l = RemainString.indexOf(str);
        RemainString = RemainString.substring(l + 1);
        i++;
    }
    int h = (u8g2.getAscent() - u8g2.getDescent()) * 1.5;
    int y = (64 - i * h) / 2 + u8g2.getAscent();
    RemainString = BlockInfo.substring(1);
    for (int j = 0; j < i; j++)
    {
        int l = RemainString.indexOf(str);
        u8g2.drawStr(0, y + h * j, ("/" + RemainString.substring(0, l)).c_str());
        RemainString = RemainString.substring(l + 1);
    }
    u8g2.sendBuffer();
}

void OLED::Save()
{
    u8g2.setFont(MSpace_Font);
    u8g2.setDrawColor(2);
    u8g2.drawStr(0, 11, "SD");
    u8g2.drawStr(14, 11, ":");
    u8g2.drawStr(21, 11, (pSD->Show.endsWith(".csv")) ? "Ready" : pSD->Show.c_str());
    u8g2.drawBox(0, 14, 128, 2);
    u8g2.drawBox(0, 18 + 16 * pSD->Cursor, 128, 14);
    int w;
    w = u8g2.getStrWidth("Back");
    u8g2.drawStr(64 - w / 2, 30, "Back");
    if (pSD->Show == "Function OFF")
    {
        w = u8g2.getStrWidth("Turn ON");
        u8g2.drawStr(64 - w / 2, 46, "Turn ON");
    }
    else
    {
        w = u8g2.getStrWidth("Turn OFF");
        u8g2.drawStr(64 - w / 2, 46, "Turn OFF");
    }
    if (pSD->Show.endsWith(".csv"))
    {
        w = u8g2.getStrWidth("File Name");
        u8g2.drawStr(64 - w / 2, 62, "File Name");
    }
}

void OLED::YesNo(bool isH, bool Select)
{
    u8g2.setFont(MSpace_Font);
    u8g2.setDrawColor(2);
    int w;
    if (isH)
    {
        u8g2.drawBox(43, 28 + 20 * Select, 42, 14);
        u8g2.drawFrame(41, 26, 46, 18);
        u8g2.drawFrame(41, 46, 46, 18);
        w = u8g2.getStrWidth("No");
        u8g2.drawStr(64 - w / 2, 40, "No");
        w = u8g2.getStrWidth("Yes");
        u8g2.drawStr(64 - w / 2, 60, "Yes");
    }
    else
    {
        u8g2.drawBox(2 + 33 * Select, 112, 26, 14);
        u8g2.drawFrame(0, 110, 30, 18);
        u8g2.drawFrame(33, 110, 30, 18);
        w = u8g2.getStrWidth("No");
        u8g2.drawStr(15 - w / 2, 124, "No");
        w = u8g2.getStrWidth("Yes");
        u8g2.drawStr(48 - w / 2, 124, "Yes");
    }
}

void OLED::Cal_M()
{
    u8g2.setFont(MSpace_Font);
    u8g2.setDrawColor(2);
    int w;
    u8g2.drawStr(0, 11, "Calibration");
    u8g2.drawBox(0, 13, 128, 2);
    u8g2.drawBox(0, 17 + 16 * (imu->Cursor - imu->CursorStart), 128, 14);
    char Cal_Mode[5][16] = {"Back", "Calibrate", "Clear", "Exp Cal", "Z Dir Exp Cal"};
    for (int i = 0; i < 3; i++)
    {
        w = u8g2.getStrWidth(Cal_Mode[i + imu->CursorStart]);
        u8g2.drawStr(64 - w / 2, 29 + i * 16, Cal_Mode[i + imu->CursorStart]);
    }
}

void OLED::Cal()
{
    u8g2.setFont(MSpace_Font);
    u8g2.setDrawColor(2);
    if (imu->CalibrateCheck == 0)
    {
        String Question;
        bool ShowYesNo = true;
        int line3 = 1;
        if (imu->Cursor == 1)
            Question = "Ready to Calibrate?";
        else if (imu->Cursor == 2)
            Question = "Sure to Clear Data?";
        else if (imu->Cursor == 3)
        {
            line3 = 0;
            Question = "g = " + String(imu->Gravity);
            ShowYesNo = false;
            if (imu->Gravity < 3)
            {
                if (imu->FullCalComplete[imu->Gravity])
                    Question += ", Complete.";
                else
                {
                    Question += ", Ready?";
                    ShowYesNo = true;
                }
            }
        }
        else if (imu->Cursor == 4)
        {
            line3 = 0;
            Question = "g = " + String(imu->Gravity);
            ShowYesNo = false;
            if (imu->Gravity % 3 == 0)
            {
                if (imu->FullCalComplete[imu->Gravity])
                    Question += ", Complete.";
                else
                {
                    Question += ", Ready?";
                    Question += "(" + String(imu->CalibrateCollectCount[imu->Gravity]+1) + ")";
                    line3 = (imu->CalibrateCollectCount[imu->Gravity] > 8) ? 4 : 3;
                    ShowYesNo = true;
                }
            }
        }

        if (Rotation % 3 != 0)
        {
            int w = 0;
            for (int i = 0; i < Question.length(); i++)
            {
                u8g2.drawGlyph(w, 13, Question.charAt(i));
                w += (Question[i] == ' ') ? 5 : 7;
            }
        }
        else
        {
            int w = 0;
            int l1 = Question.substring(7).indexOf(" ") + 8;
            u8g2.drawStr(0, 15, Question.substring(0, l1 - 1).c_str());
            for (int i = 0; i < Question.substring(l1).length() - line3; i++)
            {
                char a = Question.substring(l1).charAt(i);
                w -= (a == 'l') ? 2 : 0;
                u8g2.drawGlyph(w, 35, a);
                w += (a == ' ' || a == 'l') ? 5 : 7;
            }
            if (imu->Cursor !=3)
                u8g2.drawStr(0, 55, Question.substring(Question.length() - line3).c_str());
        }

        if (!ShowYesNo)
        {
            u8g2.drawBox(u8g2.getDisplayWidth() / 2 - 21, u8g2.getDisplayHeight() - 16, 42, 14);
            u8g2.drawFrame(u8g2.getDisplayWidth() / 2 - 23, u8g2.getDisplayHeight() - 18, 46, 18);
            u8g2.drawStr((u8g2.getDisplayWidth() - u8g2.getStrWidth("Back")) / 2, u8g2.getDisplayHeight() - 4, "Back");
        }
        else
            YesNo((Rotation % 3 != 0), imu->YesNo);
    }
    else if (imu->CalibrateCheck == 1)
    {
        for (int i = 1; i < 3; i++)
        {
            u8g2.drawGlyph(0, 17 * i - 7, 120 + ((imu->Gravity + i) % 3));
            u8g2.drawGlyph(7, 17 * i - 8, '=');
            char A[8];
            dtostrf(imu->Angle[(imu->Gravity + i) % 3], 7, 3, A);
            u8g2.drawStr(15, 17 * i - 7, A);
        }
        u8g2.drawGlyph(0, 44, 'T');
        u8g2.drawGlyph(7, 43, '=');
        u8g2.drawStr(22, 44, String(imu->SensorTemperature, 1).c_str());
        if (Rotation % 3 != 0)
        {
            u8g2.drawFrame(12, 50, 104, 14);
            u8g2.drawBox(14, 52, imu->CalibrateCount * 100 / imu->CalAvgNum, 10);
        }
        else
        {
            u8g2.drawFrame(5, 100, 54, 14);
            u8g2.drawBox(7, 102, imu->CalibrateCount * 50 / imu->CalAvgNum, 10);
        }
    }
}

int OLED::WiFiShow()
{
    if (pWifiState->now == 0)
        return 0;
    else if (pWifiState->now == pWifiState->Connected)
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

void OLED::Wifi()
{
    // Button
    // u8g2.setFont(u8g2_font_victoriabold8_8r);

    u8g2.setFont(MSpace_Font);
    if (pWifiState->Channel < WiFi_Select_Flag)
        WiFi_Select_Flag = pWifiState->Channel;
    else if (pWifiState->Channel > WiFi_Select_Flag + 3)
        WiFi_Select_Flag = pWifiState->Channel - 3;
    if (Rotation % 3 == 1)
    {
        int xb = (Rotation == 1) ? 114 : 0;
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
        int yb = (Rotation == 0) ? 0 : 114;
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
    int x[5] = {16, 41, 0, 16, 55};
    int y[5] = {52, 13, 0, 38, 13};
    u8g2.drawXBM(x[Rotation], y[Rotation], 31, 25, WiFi31x25[WiFiShow()]);
    u8g2.setFont(Describe_Font);
    char A[13];
    if (pWifiState->now == pWifiState->Off)
        strcpy(A, "Function Off");
    else if (pWifiState->now == pWifiState->On)
        strcpy(A, "No Signal");
    else if (pWifiState->now == pWifiState->ConnectingWifi)
        strcpy(A, "Connecting.");
    else if (pWifiState->now == pWifiState->Connected)
        strcpy(A, "Connected");
    int w = u8g2.getStrWidth(A);
    u8g2.drawStr(x[Rotation] + 16 - w / 2, y[Rotation] + 38, A);
}

bool OLED::Flash(int Due, int Period)
{
    return (FlashCount % Period < Due);
}
