#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "photo_data.h" 
#include "esp_wifi.h"

extern Adafruit_SSD1306 display;

// PIN JOYSTICK
#define PIN_UP    42
#define PIN_DOWN  41
#define PIN_LEFT  40
#define PIN_RIGHT 39
#define PIN_OK    38

struct WiFiData {
  int id;
  String ssid;
  int rssi;
  int channel;
  String encrypt;
  String mac;
};

// --- VARIABEL GLOBAL ---
extern int appMode;
extern bool inSubMenu;
extern int currentMenu;
extern int currentSub;
extern int topMenu;
extern bool isDeauthing;
extern bool deauthUdahSetup;
extern bool isSpamming;
extern int aktifModeSpam;
extern int spamState;
extern int brightnessValue;
extern WiFiData listWiFi[30];
extern WiFiData targetTerkunci;
extern int totalWiFi;
extern int scannerState;

// --- FIX ERROR BARU (DAFTARIN DI SINI) ---
extern int targetLockedIdx;
extern bool adaTarget;
extern int contextCursor;
extern int deauthState;
extern bool scanDone;
extern bool triggerScan;
extern bool sedang_scan;
extern int cursorInScanner;
extern int scrollPosScanner;

// --- PROTOTYPE FUNGSI (Lengkapin biar gak error 'not declared') ---
void handleJoystick();
void tampilkanMenuLogo();
void tampilkanMenuUtama();
void tampilkanWifiScanner();
void tampilkanDeauthScreen();    // Baru
void tampilkanBrightness();      // Baru
void tampilkanSpamScreen(String judul, String subTeks); // Baru
void loopWiFi(void * pvParameters);
void tampilkanLogoDulu();
void tampilkanIntroAnime();
void tampilkanTeksSplash();
void handleNavigasiScanner(String btn);
void handleNavigasiDeauth(String btn);
void handleNavigasiSpam(String btn);
void setOledBrightness(uint8_t level);
void ketikTeks(String teks, int x, int y);

#endif
