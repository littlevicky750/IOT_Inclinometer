#ifndef OLED_XBM_H
#define OLED_XBM_H

static unsigned char Waiting13x17[] = {
    0xFF, 0x1F, 0x02, 0x08, 0x02, 0x08, 0x52, 0x09, 0xA4, 0x04, 0x48, 0x02,
    0x10, 0x01, 0xA0, 0x00, 0x40, 0x00, 0xA0, 0x00, 0x10, 0x01, 0x48, 0x02,
    0x04, 0x04, 0xA2, 0x08, 0x52, 0x09, 0xAA, 0x0A, 0xFF, 0x1F};
static unsigned char Up10x3[] = {0x30, 0x00, 0xFC, 0x00, 0xFF, 0x03};
static unsigned char Down10x3[] = {0xFF, 0x03, 0xFC, 0x00, 0x30, 0x00};
static unsigned char Left3x10[] = {0x04, 0x04, 0x06, 0x06, 0x07, 0x07, 0x06, 0x06, 0x04, 0x04};
static unsigned char Right3x10[] = {0x01, 0x01, 0x03, 0x03, 0x07, 0x07, 0x03, 0x03, 0x01, 0x01};

static unsigned char Fire24x24[] = {
    0x00, 0x06, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x38, 0x00, 0x00, 0x78, 0x00,
    0x00, 0xF8, 0x00, 0x00, 0xFC, 0x01, 0x00, 0xFE, 0x03, 0x00, 0xFF, 0x03,
    0x80, 0xFF, 0x07, 0xC0, 0xFF, 0x07, 0xC4, 0xFF, 0x27, 0xE6, 0xFF, 0x73,
    0xE7, 0xEF, 0xFB, 0xEF, 0xF7, 0xFF, 0xFF, 0xF3, 0xFF, 0xFF, 0xE3, 0xFF,
    0xFF, 0xC3, 0x7F, 0x9F, 0x83, 0x7D, 0x3F, 0x01, 0x7E, 0x3E, 0x00, 0x3E,
    0x7C, 0x00, 0x1F, 0xFC, 0x00, 0x1F, 0xF0, 0x83, 0x0F, 0xC0, 0xE7, 0x03};
static unsigned char Ready24x24[] = {
    0x80, 0x81, 0x01, 0xE0, 0x81, 0x07, 0xF0, 0xC3, 0x0F, 0xF0, 0xFF, 0x0F,
    0xE0, 0xFF, 0x07, 0xE0, 0xC3, 0x07, 0xF0, 0x00, 0x0F, 0x70, 0x00, 0x0E,
    0x38, 0x00, 0x1C, 0x3F, 0x80, 0xFC, 0x1F, 0xC0, 0xF9, 0x1F, 0xE0, 0xF8,
    0x1F, 0x71, 0xF8, 0x9F, 0x3B, 0xF8, 0x3F, 0x1F, 0xFC, 0x38, 0x0E, 0x1C,
    0x70, 0x04, 0x0E, 0xF0, 0x00, 0x0F, 0xE0, 0xC3, 0x07, 0xE0, 0xFF, 0x07,
    0xF0, 0xFF, 0x0F, 0xF0, 0xC3, 0x0F, 0xE0, 0x81, 0x07, 0x80, 0x81, 0x01};
static unsigned char Done24x24[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x78,
    0x00, 0x00, 0xFC, 0x00, 0x00, 0xFE, 0x00, 0x00, 0x7F, 0x00, 0x80, 0x3F,
    0x00, 0xC0, 0x1F, 0x00, 0xE0, 0x0F, 0x00, 0xF0, 0x07, 0x0C, 0xF8, 0x03,
    0x1E, 0xFC, 0x01, 0x3F, 0xFE, 0x00, 0x7F, 0x7F, 0x00, 0xFE, 0x3F, 0x00,
    0xFC, 0x1F, 0x00, 0xF8, 0x0F, 0x00, 0xF0, 0x07, 0x00, 0xE0, 0x03, 0x00,
    0xC0, 0x01, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static unsigned char SD24x24[] = {
    0xC0, 0xFF, 0x0F, 0xE0, 0xFF, 0x1F, 0xE0, 0x79, 0x1E, 0x20, 0x49, 0x12,
    0x20, 0x49, 0x12, 0x20, 0x49, 0x12, 0x20, 0x49, 0x12, 0x20, 0x49, 0x12,
    0xF0, 0xFF, 0x1F, 0xF8, 0xFF, 0x1F, 0xF8, 0xFF, 0x1F, 0xF8, 0xFF, 0x1F,
    0xE0, 0xFF, 0x1F, 0xE0, 0xFF, 0x1F, 0xF0, 0xFF, 0x1F, 0xF8, 0xFF, 0x1F,
    0xF8, 0xFF, 0x1F, 0xF8, 0xFF, 0x1F, 0xF8, 0xFF, 0x1F, 0xF8, 0xFF, 0x1F,
    0xF8, 0xFF, 0x1F, 0xF8, 0xFF, 0x1F, 0x18, 0x00, 0x18, 0xF0, 0xFF, 0x0F};
static unsigned char NOSD24x24[] = {
    0xC0, 0xFF, 0x0F, 0xE0, 0xFF, 0x1F, 0xE0, 0x79, 0x1E, 0x20, 0x49, 0x12,
    0x20, 0x49, 0x12, 0x20, 0x49, 0x12, 0x20, 0x49, 0x12, 0x20, 0x49, 0x12,
    0xF0, 0xFF, 0x1F, 0xF8, 0xFF, 0x1F, 0xF8, 0xFF, 0x1F, 0xF8, 0x7D, 0x1F,
    0xE0, 0x38, 0x1E, 0xE0, 0x11, 0x1F, 0xF0, 0x83, 0x1F, 0xF8, 0xC7, 0x1F,
    0xF8, 0x83, 0x1F, 0xF8, 0x11, 0x1F, 0xF8, 0x38, 0x1E, 0xF8, 0x7D, 0x1F,
    0xF8, 0xFF, 0x1F, 0xF8, 0xFF, 0x1F, 0x18, 0x00, 0x18, 0xF0, 0xFF, 0x0F};
static unsigned char SDSave24x24[] = {
    0xC0, 0xFF, 0x0F, 0xE0, 0xFF, 0x1F, 0xE0, 0x79, 0x1E, 0x20, 0x49, 0x12,
    0x20, 0x49, 0x12, 0x20, 0x49, 0x12, 0x20, 0x49, 0x12, 0x20, 0x49, 0x12,
    0xF0, 0xFF, 0x1F, 0xF8, 0xFF, 0x1F, 0xF8, 0xFF, 0x1F, 0xF8, 0xEF, 0x1F,
    0xE0, 0xEF, 0x1F, 0xE0, 0xEF, 0x1F, 0xF0, 0xEF, 0x1F, 0xF8, 0xEF, 0x1F,
    0xF8, 0x6D, 0x1F, 0xF8, 0xAB, 0x1F, 0xF8, 0xC7, 0x1F, 0xF8, 0xEF, 0x1F,
    0xF8, 0xFF, 0x1F, 0xF8, 0xFF, 0x1F, 0x18, 0x00, 0x18, 0xF0, 0xFF, 0x0F};

static unsigned char Timer0_24x24[] = {
    0xFC, 0xFF, 0x3F, 0xFC, 0xFF, 0x3F, 0x10, 0x00, 0x08, 0x10, 0x00, 0x08,
    0x10, 0x00, 0x09, 0xA0, 0xC1, 0x05, 0x20, 0xF7, 0x04, 0x40, 0x7E, 0x02,
    0x80, 0x3C, 0x01, 0x00, 0x99, 0x00, 0x00, 0x42, 0x00, 0x00, 0x24, 0x00,
    0x00, 0x24, 0x00, 0x00, 0x42, 0x00, 0x00, 0x91, 0x00, 0x80, 0x00, 0x01,
    0x40, 0x08, 0x02, 0x20, 0x00, 0x04, 0x20, 0x7E, 0x04, 0x90, 0xFF, 0x09,
    0xD0, 0xFF, 0x0B, 0xD0, 0xFF, 0x0B, 0xFC, 0xFF, 0x3F, 0xFC, 0xFF, 0x3F};
static unsigned char Timer1_24x24[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0xC0, 0x03, 0x00, 0xC0,
    0x1F, 0x00, 0xF8, 0x63, 0x00, 0xC6, 0x8F, 0x00, 0xC1, 0x1F, 0x81, 0xC4,
    0x1F, 0x42, 0xC6, 0x3F, 0x24, 0xC3, 0x3F, 0x98, 0xC3, 0xBF, 0xC0, 0xC1,
    0x3F, 0xC2, 0xC3, 0x3F, 0x98, 0xC3, 0x3F, 0x24, 0xC7, 0x1F, 0x42, 0xC6,
    0x1F, 0x81, 0xCC, 0x8F, 0x00, 0xC1, 0x63, 0x00, 0xC6, 0x1F, 0x00, 0xF8,
    0x03, 0x00, 0xC0, 0x03, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static unsigned char Timer2_24x24[] = {
    0xFC, 0xFF, 0x3F, 0xFC, 0xFF, 0x3F, 0x10, 0x00, 0x08, 0x10, 0x00, 0x08,
    0x90, 0x00, 0x08, 0xA0, 0x83, 0x05, 0x20, 0xEF, 0x04, 0x40, 0x7E, 0x02,
    0x80, 0x3C, 0x01, 0x00, 0x99, 0x00, 0x00, 0x42, 0x00, 0x00, 0x24, 0x00,
    0x00, 0x24, 0x00, 0x00, 0x42, 0x00, 0x00, 0x89, 0x00, 0x80, 0x00, 0x01,
    0x40, 0x10, 0x02, 0x20, 0x00, 0x04, 0x20, 0x7E, 0x04, 0x90, 0xFF, 0x09,
    0xD0, 0xFF, 0x0B, 0xD0, 0xFF, 0x0B, 0xFC, 0xFF, 0x3F, 0xFC, 0xFF, 0x3F};
static unsigned char Timer3_24x24[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0xC0, 0x03, 0x00, 0xC0,
    0x1F, 0x00, 0xF8, 0x63, 0x00, 0xC6, 0x8F, 0x00, 0xC1, 0x1F, 0x81, 0xCC,
    0x1F, 0x42, 0xC6, 0x3F, 0x24, 0xC7, 0x3F, 0x98, 0xC3, 0x3F, 0xC2, 0xC3,
    0xBF, 0xC0, 0xC1, 0x3F, 0x98, 0xC3, 0x3F, 0x24, 0xC3, 0x1F, 0x42, 0xC6,
    0x1F, 0x81, 0xC4, 0x8F, 0x00, 0xC1, 0x63, 0x00, 0xC6, 0x1F, 0x00, 0xF8,
    0x03, 0x00, 0xC0, 0x03, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static unsigned char Clock24x24[] = {
    0x00, 0x7E, 0x00, 0xC0, 0xFF, 0x03, 0xE0, 0x81, 0x07, 0x70, 0x08, 0x0E,
    0x18, 0x08, 0x18, 0x0C, 0x08, 0x38, 0x0E, 0x08, 0x70, 0x06, 0x08, 0x60,
    0x06, 0x08, 0x60, 0x03, 0x08, 0xC0, 0x03, 0x08, 0xC0, 0x03, 0x08, 0xC0,
    0x03, 0x10, 0xC0, 0x03, 0x20, 0xC0, 0x03, 0x40, 0xC0, 0x06, 0x80, 0x60,
    0x06, 0x00, 0x60, 0x0E, 0x00, 0x70, 0x0C, 0x00, 0x30, 0x38, 0x00, 0x18,
    0x70, 0x00, 0x0E, 0xE0, 0x81, 0x07, 0xC0, 0xFF, 0x03, 0x00, 0x7E, 0x00};
static unsigned char Home24x24[] = {
    0x00, 0x18, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x7E, 0x00, 0x00, 0xFF, 0x00,
    0x80, 0xFF, 0x01, 0xC0, 0xFF, 0x03, 0xE0, 0xFF, 0x07, 0xF0, 0xFF, 0x0F,
    0xF8, 0xFF, 0x1F, 0xFC, 0xFF, 0x3F, 0xFE, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF,
    0x08, 0x00, 0x10, 0x08, 0x00, 0x10, 0x08, 0x00, 0x10, 0x08, 0x7E, 0x10,
    0x08, 0x7E, 0x10, 0x08, 0x7E, 0x10, 0x08, 0x7E, 0x10, 0x08, 0x7E, 0x10,
    0x08, 0x7E, 0x10, 0x08, 0x7E, 0x10, 0x08, 0x7E, 0x10, 0xF8, 0xFF, 0x1F};

static unsigned char WifiHigh31x25[] = {
    0x00, 0xF8, 0x0F, 0x00, 0x00, 0xFF, 0x7F, 0x00, 0xC0, 0xFF, 0xFF, 0x01,
    0xF0, 0x0F, 0xF0, 0x07, 0xF8, 0x00, 0x80, 0x1F, 0x7E, 0x00, 0x00, 0x3E,
    0x1F, 0xF0, 0x07, 0x7C, 0x0E, 0xFE, 0x3F, 0x38, 0x80, 0xFF, 0xFF, 0x00,
    0xC0, 0x0F, 0xF8, 0x01, 0xE0, 0x03, 0xE0, 0x03, 0xE0, 0x00, 0x80, 0x03,
    0x40, 0xE0, 0x03, 0x01, 0x00, 0xF8, 0x0F, 0x00, 0x00, 0xFE, 0x3F, 0x00,
    0x00, 0x1E, 0x3C, 0x00, 0x00, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xC0, 0x01, 0x00, 0x00, 0xE0, 0x03, 0x00, 0x00, 0xF0, 0x07, 0x00,
    0x00, 0xF0, 0x07, 0x00, 0x00, 0xF0, 0x07, 0x00, 0x00, 0xE0, 0x03, 0x00,
    0x00, 0xC0, 0x01, 0x00};
static unsigned char WifiMiddle31x25[] = {
    0x00, 0xF8, 0x0F, 0x00, 0x00, 0x07, 0x70, 0x00, 0xC0, 0xF0, 0x8F, 0x01,
    0x30, 0x0F, 0x70, 0x06, 0x88, 0x00, 0x80, 0x19, 0x66, 0x00, 0x00, 0x22,
    0x11, 0xF0, 0x07, 0x44, 0x0E, 0xFE, 0x3F, 0x38, 0x80, 0xFF, 0xFF, 0x00,
    0xC0, 0x0F, 0xF8, 0x01, 0xE0, 0x03, 0xE0, 0x03, 0xE0, 0x00, 0x80, 0x03,
    0x40, 0xE0, 0x03, 0x01, 0x00, 0xF8, 0x0F, 0x00, 0x00, 0xFE, 0x3F, 0x00,
    0x00, 0x1E, 0x3C, 0x00, 0x00, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xC0, 0x01, 0x00, 0x00, 0xE0, 0x03, 0x00, 0x00, 0xF0, 0x07, 0x00,
    0x00, 0xF0, 0x07, 0x00, 0x00, 0xF0, 0x07, 0x00, 0x00, 0xE0, 0x03, 0x00,
    0x00, 0xC0, 0x01, 0x00};
static unsigned char WifiLow31x25[] = {
    0x00, 0xF8, 0x0F, 0x00, 0x00, 0x07, 0x70, 0x00, 0xC0, 0xF0, 0x8F, 0x01,
    0x30, 0x0F, 0x70, 0x06, 0x88, 0x00, 0x80, 0x19, 0x66, 0x00, 0x00, 0x22,
    0x11, 0xF0, 0x07, 0x44, 0x0E, 0x0E, 0x38, 0x38, 0x80, 0xF1, 0xC7, 0x00,
    0x40, 0x0C, 0x18, 0x01, 0x20, 0x03, 0x60, 0x02, 0xA0, 0x00, 0x80, 0x02,
    0x40, 0xE0, 0x03, 0x01, 0x00, 0xF8, 0x0F, 0x00, 0x00, 0xFE, 0x3F, 0x00,
    0x00, 0x1E, 0x3C, 0x00, 0x00, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xC0, 0x01, 0x00, 0x00, 0xE0, 0x03, 0x00, 0x00, 0xF0, 0x07, 0x00,
    0x00, 0xF0, 0x07, 0x00, 0x00, 0xF0, 0x07, 0x00, 0x00, 0xE0, 0x03, 0x00,
    0x00, 0xC0, 0x01, 0x00};
static unsigned char WifiOn31x25[] = {
    0x00, 0xF8, 0x0F, 0x00, 0x00, 0x07, 0x70, 0x00, 0xC0, 0xF0, 0x8F, 0x01,
    0x30, 0x0F, 0x70, 0x06, 0x88, 0x00, 0x80, 0x19, 0x66, 0x00, 0x00, 0x22,
    0x11, 0xF0, 0x07, 0x44, 0x0E, 0x0E, 0x38, 0x38, 0x80, 0xF1, 0xC7, 0x00,
    0x40, 0x0C, 0x18, 0x01, 0x20, 0x03, 0x60, 0x02, 0xA0, 0x00, 0x80, 0x02,
    0x40, 0xE0, 0x03, 0x01, 0x00, 0x18, 0x0C, 0x00, 0x00, 0xE6, 0x33, 0x00,
    0x00, 0x12, 0x24, 0x00, 0x00, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xC0, 0x01, 0x00, 0x00, 0xE0, 0x03, 0x00, 0x00, 0xF0, 0x07, 0x00,
    0x00, 0xF0, 0x07, 0x00, 0x00, 0xF0, 0x07, 0x00, 0x00, 0xE0, 0x03, 0x00,
    0x00, 0xC0, 0x01, 0x00};
static unsigned char WifiOff31x25[] = {
    0x00, 0xF8, 0x0F, 0x00, 0x00, 0x07, 0x70, 0x00, 0xC0, 0xF0, 0x8F, 0x01,
    0x30, 0x0F, 0x70, 0x06, 0x88, 0x00, 0x80, 0x19, 0x66, 0x00, 0x00, 0x22,
    0x11, 0xF0, 0x07, 0x44, 0x0E, 0x0E, 0x38, 0x38, 0x80, 0xF1, 0xC7, 0x00,
    0x40, 0x0C, 0x18, 0x01, 0x20, 0x03, 0x60, 0x02, 0xA0, 0x00, 0x80, 0x02,
    0x40, 0xE0, 0x03, 0x01, 0x00, 0x18, 0x0C, 0x00, 0x00, 0xE6, 0x33, 0x00,
    0x00, 0x12, 0x24, 0x00, 0x00, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xC0, 0x01, 0x00, 0x00, 0x20, 0x02, 0x00, 0x00, 0x10, 0x04, 0x00,
    0x00, 0x10, 0x04, 0x00, 0x00, 0x10, 0x04, 0x00, 0x00, 0x20, 0x02, 0x00,
    0x00, 0xC0, 0x01, 0x00};

static unsigned char WiFi0_24x24[] = {
    0x00, 0xFF, 0x00, 0xE0, 0x00, 0x07, 0x18, 0xFF, 0x18, 0xC6, 0x00, 0x63,
    0x31, 0x00, 0x8C, 0x09, 0x7E, 0x90, 0xC6, 0x81, 0x63, 0x30, 0x7E, 0x0C,
    0x88, 0x81, 0x11, 0x50, 0x00, 0x0A, 0x20, 0x7E, 0x04, 0x00, 0x81, 0x00,
    0x80, 0x3C, 0x01, 0x00, 0xC3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00,
    0x00, 0x42, 0x00, 0x00, 0x42, 0x00, 0x00, 0x42, 0x00, 0x00, 0x42, 0x00,
    0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static unsigned char WiFi1_24x24[] = {
    0x00, 0xFF, 0x00, 0xE0, 0x00, 0x07, 0x18, 0xFF, 0x18, 0xC6, 0x00, 0x63,
    0x31, 0x00, 0x8C, 0x09, 0x7E, 0x90, 0xC6, 0x81, 0x63, 0x30, 0x7E, 0x0C,
    0x88, 0x81, 0x11, 0x50, 0x00, 0x0A, 0x20, 0x7E, 0x04, 0x00, 0x81, 0x00,
    0x80, 0x3C, 0x01, 0x00, 0xC3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00,
    0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00,
    0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static unsigned char WiFi2_24x24[] = {
    0x00, 0xFF, 0x00, 0xE0, 0x00, 0x07, 0x18, 0xFF, 0x18, 0xC6, 0x00, 0x63,
    0x31, 0x00, 0x8C, 0x09, 0x7E, 0x90, 0xC6, 0x81, 0x63, 0x30, 0x7E, 0x0C,
    0x88, 0x81, 0x11, 0x50, 0x00, 0x0A, 0x20, 0x7E, 0x04, 0x00, 0xFF, 0x00,
    0x80, 0xFF, 0x01, 0x00, 0xC3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00,
    0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00,
    0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static unsigned char WiFi3_24x24[] = {
    0x00, 0xFF, 0x00, 0xE0, 0x00, 0x07, 0x18, 0xFF, 0x18, 0xC6, 0x00, 0x63,
    0x31, 0x00, 0x8C, 0x09, 0x7E, 0x90, 0xC6, 0xFF, 0x63, 0xF0, 0xFF, 0x0F,
    0xF8, 0x81, 0x1F, 0x70, 0x00, 0x0E, 0x20, 0x7E, 0x04, 0x00, 0xFF, 0x00,
    0x80, 0xFF, 0x01, 0x00, 0xC3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00,
    0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00,
    0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static unsigned char WiFi4_24x24[] = {
    0x00, 0xFF, 0x00, 0xE0, 0xFF, 0x07, 0xF8, 0xFF, 0x1F, 0xFE, 0x00, 0x7F,
    0x3F, 0x00, 0xFC, 0x0F, 0x7E, 0xF0, 0xC6, 0xFF, 0x63, 0xF0, 0xFF, 0x0F,
    0xF8, 0x81, 0x1F, 0x70, 0x00, 0x0E, 0x20, 0x7E, 0x04, 0x00, 0xFF, 0x00,
    0x80, 0xFF, 0x01, 0x00, 0xC3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00,
    0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00,
    0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char *Menu_XBM[6] = {&Home24x24[0], &Home24x24[0], &Home24x24[0], &Home24x24[0], &Clock24x24[0], &SD24x24[0]};
unsigned char *WiFi31x25[5] = {&WifiOff31x25[0], &WifiOn31x25[0], &WifiLow31x25[0], &WifiMiddle31x25[0], &WifiHigh31x25[0]};
unsigned char *WiFi24x24[5] = {&WiFi0_24x24[0], &WiFi1_24x24[0], &WiFi2_24x24[0], &WiFi3_24x24[0], &WiFi4_24x24[0]};
#endif