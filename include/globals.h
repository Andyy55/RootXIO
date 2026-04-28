#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "esp_wifi.h"

// --- OBJEK DISPLAY (Biar semua file bisa gambar ke layar) ---
extern Adafruit_SSD1306 display;

// --- STRUKTUR DATA ---
struct WiFiData {
  int id;
  String ssid;
  int rssi;
  int channel;
  String encrypt;
  String mac;
};

// --- VARIABEL GLOBAL (Gunakan 'extern') ---
extern int appMode;
extern bool isDeauthing;
extern bool isSpamming;
extern int aktifModeSpam;
extern WiFiData targetTerkunci;
extern WiFiData listWiFi[30];
extern int totalWiFi;
extern bool deauthUdahSetup;

// --- PROTOTYPE FUNGSI (Biar file lain bisa panggil fungsi ini) ---
void handleJoystick();
void tampilkanMenuLogo();
void loopWiFi(void * pvParameters);
void sendBeacon(String ssid);
void setOledBrightness(uint8_t level);

#endif
