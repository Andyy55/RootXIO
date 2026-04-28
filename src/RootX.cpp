#include <Arduino.h>
#include <globals.h>
#include <Wire.h>
#include "esp_wifi.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "photo_data.h"
#include <WiFi.h>


// --- SETTING LAYAR ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- SETTING PIN JOYSTICK (S3 N16R8 Lu) ---
#define PIN_UP    42
#define PIN_DOWN  41
#define PIN_LEFT  40
#define PIN_RIGHT 39
#define PIN_OK    38

// --- VARIABEL MENU GLOBAL (Biar semua file kenal) ---
bool inSubMenu = false;    // Status: lagi di Logo (false) atau di List (true)
int currentMenu = 0;       // Pilih kategori logo (0-3)
int currentSub = 0;        // Pilih baris menu di dalem list
int topMenu = 0;           // Buat scrolling list menu

// Struktur data WiFi biar bisa dibaca di semua file
struct WiFiData {
  int id;
  String ssid;
  int rssi;
  int channel;
  String encrypt;
  String mac;
};





// Variabel Global Scanner
WiFiData listWiFi[30];


int brightnessValue = 150; // Default (0-255)

int spamState = 0; 
bool isSpamming = false;
int aktifModeSpam = 0;

bool spamUdahSetup = false;
bool deauthUdahSetup = false;

bool scanDone = false;
bool triggerScan = false;
bool sedang_scan = false;
int cursorInScanner = 0;
int scrollPosScanner = 0;
// --- Variabel State WiFi Scanner ---
int scannerState = 0; 
unsigned long popUpTimer = 0; 
bool triggerScan = false; 
bool scanDone = false;    
int totalWiFi = 0;
int cursorInScanner = 0; 
int scrollPosScanner = 0;
int targetLockedIdx = -1;
int contextCursor = 0;

// --- BRANKAS TARGET ---
WiFiData targetTerkunci; 
bool adaTarget = false;  

// --- DEAUTH VAR ----
int deauthState = 0;
bool isDeauthing = false;

// ----------------------

bool sedang_scan = false;
int appMode = 0;

TaskHandle_t TaskWiFi; // Handle untuk Core 0

void setup() {
  Serial.begin(115200);

  // Setup Input
  pinMode(PIN_UP, INPUT_PULLUP);
  pinMode(PIN_DOWN, INPUT_PULLUP);
  pinMode(PIN_LEFT, INPUT_PULLUP);
  pinMode(PIN_RIGHT, INPUT_PULLUP);
  pinMode(PIN_OK, INPUT_PULLUP);

  // Setup I2C buat S3 N16R8 (PINnya)
  Wire.begin(8, 9);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for(;;);
  }

  // --- BARIS SAKTI BUAT MUTER LAYAR 180 DERAJAT ---
  display.setRotation(2); 
  // 0 = Normal, 1 = 90 deg, 2 = 180 deg (Kebalik), 3 = 270 deg
  
  display.clearDisplay(); // Bersihin sisa-sisa tampilan lama
  

  // --- BOOTING ---
  tampilkanLogoDulu();    // 1. Logo Mahkota + Nama Andy (Identitas Awal)
  tampilkanIntroAnime();  // 2. Glitch -> Foto Anime + Firmware Info (Identity)
  tampilkanTeksSplash();  // 3. Loading Ngetik (Hardware Check)
  
    // --- Membuat Task untuk WiFi Scanner (Core 0) ---
  xTaskCreatePinnedToCore(
    loopWiFi,     /* Fungsi task */
    "TaskWiFi",   /* Nama task */
    10000,        /* Stack size */
    NULL,         /* Parameter */
    1,            /* Prioritas */
    &TaskWiFi,    /* Handle */
    0             /* Core 0 */
  );
  
}

void loop() {
  handleJoystick(); // Cek input dari input_system.ino

  // Cek sekarang lagi buka apa
  if (appMode == 0) {
    // Kalau appMode 0, berarti lagi di area HOME / Menu Utama
    if (inSubMenu == false) {
      tampilkanMenuLogo();  // Tampilan Logo Gede + Panah < >
    } else {
      tampilkanMenuUtama(); // Tampilan List Menu kebawah + Ikon Kecil
    }
  } 
  else if (appMode == 1) {
    // Kalau appMode 1, berarti lagi buka "Aplikasi" WiFi Scanner
    tampilkanWifiScanner(); 
  } else if (appMode == 2) {
    tampilkanDeauthScreen(); 
  } else if (appMode == 3) { 
  tampilkanBrightness();
} else if (appMode == 4) { // <--- INI DIA!
    if (aktifModeSpam == 1) tampilkanSpamScreen("BEACON SPAM", "Start Spam?");
    else if (aktifModeSpam == 2) tampilkanSpamScreen("RICKROLL", "Start Spam?");
  }
}
