#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "photo_data.h" // WAJIB biar ikon kedetect
#include "esp_wifi.h"

// --- SETTING LAYAR ---
extern Adafruit_SSD1306 display;

// --- PIN JOYSTICK ---
#define PIN_UP    42
#define PIN_DOWN  41
#define PIN_LEFT  40
#define PIN_RIGHT 39
#define PIN_OK    38

// --- STRUKTUR DATA ---
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
extern int scannerState; // Tambahin ini karena di log error ada scannerState

// --- PROTOTYPE FUNGSI (Biar RootX.cpp bisa panggil file lain) ---
void handleJoystick();
void tampilkanMenuLogo();
void tampilkanMenuUtama();
void tampilkanWifiScanner();
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
