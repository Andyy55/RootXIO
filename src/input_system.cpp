#include <Arduino.h>
#include <globals.h>

void handleJoystick() {
  static unsigned long lastPress = 0;
  if (millis() - lastPress < 250) return; 

  // --- 1. TENTUIN DULU BTN NYA (Biar yang pendek bisa jalan) ---
  String btn = "NONE";
  if (digitalRead(PIN_UP) == LOW)    btn = "UP";
  else if (digitalRead(PIN_DOWN) == LOW)  btn = "DOWN";
  else if (digitalRead(PIN_LEFT) == LOW)  btn = "LEFT";
  else if (digitalRead(PIN_RIGHT) == LOW) btn = "RIGHT";
  else if (digitalRead(PIN_OK) == LOW)    btn = "OK";

  if (btn == "NONE") return; 

  // --- 2. PANGGIL APPMODE (Paket Hemat) ---
  if (appMode == 1) { handleNavigasiScanner(btn); lastPress = millis(); return; }
  if (appMode == 2) { handleNavigasiDeauth(btn);  lastPress = millis(); return; }
  if (appMode == 4) { handleNavigasiSpam(btn);    lastPress = millis(); return; } // <--- PENDEK & SAKTI
  
  // Mode 3 (Brightness) karena dia gak pake fungsi handle luar, tulis gini aja:
  if (appMode == 3) {
    if (btn == "UP") {
      if (brightnessValue < 245) brightnessValue += 10;
      setOledBrightness(brightnessValue);
    }
    else if (btn == "DOWN") {
      if (brightnessValue > 10) brightnessValue -= 10;
      setOledBrightness(brightnessValue);
    }
    else if (btn == "LEFT") appMode = 0;
    
    lastPress = millis();
    return;
  }

  // ... sisa kodingan menu utama di bawah ...


  // --- BATAS TAMBAHAN ---

  
  if (!inSubMenu) {
    // ==========================================
    // MODE 1: CAROUSEL LOGO (KANAN - KIRI)
    // ==========================================
    if (btn == "RIGHT") {
      currentMenu = (currentMenu + 1) % 4; // Ganti 4 sesuai jumlah logo lu
      lastPress = millis();
    }
    else if (btn == "LEFT") {
      currentMenu = (currentMenu - 1 + 4) % 4; 
      lastPress = millis();
    }
    else if (btn == "OK") {
      inSubMenu = true; // MASUK KE LIST MENU
      currentSub = 0;   // Reset kursor biar mulai dari atas
      topMenu = 0;
      lastPress = millis();
    }
  } 
  else {
  
  
  
    // ==========================================
    // MODE 2: LIST MENU (ATAS - BAWAH)
    // ==========================================
    // Di dalam handleJoystick bagian Mode List (inSubMenu == true)
// Di input_system.ino dalam handleJoystick()
if (btn == "DOWN") {
  int limitMenu = 0; // Ganti nama biar gak bentrok
  if(currentMenu == 0)      limitMenu = 4; 
  else if(currentMenu == 1) limitMenu = 3;
  else if(currentMenu == 2) limitMenu = 5;
  else                      limitMenu = 4;

  if (currentSub < (limitMenu - 1)) { // Tambahin kurung biar aman
    currentSub++;
    if (currentSub >= topMenu + 5) topMenu++;
  }
  lastPress = millis();
}


    else if (btn == "UP") {
      if (currentSub > 0) {
        currentSub--;
        if (currentSub < topMenu) topMenu--;
      }
      lastPress = millis();
    }
    // --- TOMBOL KIRI UNTUK BACK ---
    else if (btn == "LEFT") {
      inSubMenu = false; // KELUAR BALIK KE LOGO
      lastPress = millis();
    }
   else if (btn == "OK") {
     
      if (currentMenu == 0 && currentSub == 0) {
        
        appMode = 1;      
        scannerState = 0; 
        
      } else if (currentMenu == 0 && currentSub == 2) {
        appMode = 1;
        scannerState = 2;     // LANGSUNG LOMPAT KE HASIL LIST! (Bypass Scan)
        cursorInScanner = 0;  // Reset kursor layar
        scrollPosScanner = 0; // Reset scroll
      } else if (currentMenu == 3 && currentSub == 0) { // Menu Settings -> Brightness
    appMode = 3; 
} else if (currentMenu == 0 && currentSub == 1) {
    aktifModeSpam = 1; // ID Beacon
    appMode = 4;       // Masuk ke layar konfirmasi spam
    spamState = 0;
}
// Contoh pas klik menu RickRoll
else if (currentMenu == 0 && currentSub == 3) {
    aktifModeSpam = 2; // ID Rickroll
    appMode = 4;
    spamState = 0;
}

     
      
      lastPress = millis();
    }
  }
  }


// ==========================================
// LOGIKA NAVIGASI KHUSUS WIFI SCANNER
// ==========================================
void handleNavigasiScanner(String btn) {
  // --- LOGIKA SAAT DI MENU KONFIRMASI (STATE 0) ---
  if (scannerState == 0) {
    if (btn == "LEFT") {
      appMode = 0; // Batal, balik ke list menu utama
    } 
    else if (btn == "RIGHT" || btn == "OK") { // Tombol YES ditekan
      scannerState = 1;     // Pindah ke layar loading
      triggerScan = true;   // Tarik pelatuk Core 0!
      scanDone = false;     // Reset flag
      cursorInScanner = 0;  // Reset kursor
      scrollPosScanner = 0; // Reset scroll
    }
  }
  // --- LOGIKA SAAT LOADING SCAN (STATE 1) ---
  else if (scannerState == 1) {
    // Biarin tombol mati rasa pas lagi scan biar gak error
    // Atau kalau mau bisa dibatalin:
    if (btn == "LEFT") {
      scannerState = 0; // Batalin nunggu, balik ke konfirmasi
    }
  }
    // --- LOGIKA SAAT DI HASIL LIST (STATE 2) ---
  else if (scannerState == 2) {
    if (btn == "UP") {
      if (cursorInScanner > 0) cursorInScanner--;
      else if (scrollPosScanner > 0) scrollPosScanner--;
    } 
    else if (btn == "DOWN") {
      if (cursorInScanner < 2 && (scrollPosScanner + cursorInScanner) < (totalWiFi - 1)) cursorInScanner++;
      else if ((scrollPosScanner + 3) < totalWiFi) scrollPosScanner++;
    }
    // CUMA TOMBOL "OK" YANG BISA (Tombol Kanan/SELECT dimatiin)
    else if (btn == "OK") {
      if (totalWiFi > 0) {
        targetLockedIdx = scrollPosScanner + cursorInScanner; 
        targetTerkunci = listWiFi[targetLockedIdx];
        adaTarget = true; 
        
        scannerState = 4;   // PINDAH KE MENU CONTEXT
        contextCursor = 0;  // Reset kursor context ke paling atas
      }
    }
    else if (btn == "LEFT") {
      scannerState = 0; 
      appMode = 0;      
    }
  }
    // --- LOGIKA SAAT DI DETAIL VIEW (STATE 3) ---
  else if (scannerState == 3) {
    if (btn == "LEFT") { // Tambahin LEFT biar aman
      scannerState = 4; // Balik ke Menu Context (Actions)
      // Kalau mau langsung balik ke list WiFi, ganti jadi: scannerState = 2;
    }
  }
  
  // --- LOGIKA SAAT DI MENU CONTEXT (STATE 4) ---
  else if (scannerState == 4) {
    if (btn == "UP") {
      // Fitur Scroll Menu Context (Looping)
      contextCursor = (contextCursor > 0) ? contextCursor - 1 : 1; 
    }
    else if (btn == "DOWN") {
      contextCursor = (contextCursor < 1) ? contextCursor + 1 : 0;
    }
    else if (btn == "OK") {
      if (contextCursor == 0) {
        appMode = 2;     // LOMPAT KE ATTACK DEAUTH
        deauthState = 0; // Mulai dari konfirmasi
      } 
      else if (contextCursor == 1) {
        scannerState = 3; // LOMPAT KE DETAIL VIEW
      }
    }
    else if (btn == "LEFT") {
      scannerState = 2; // Batal, balik ke list WiFi
    }
  }
}

void handleNavigasiDeauth(String btn) {
  if (deauthState == 0) { // Layar Konfirmasi
    if (btn == "LEFT") appMode = 0; 
    else if (btn == "RIGHT" || btn == "OK") { // Biar dua-duanya bisa
      deauthState = 1;
      isDeauthing = true;
    }

  } 
  else if (deauthState == 1) { // Lagi Attack
    if (btn == "LEFT") { // Stop Attack
      isDeauthing = false;
      deauthState = 0;
      appMode = 0; // Balik ke menu
    }
  }
}

void handleNavigasiSpam(String btn) {
  if (spamState == 0) { // Lagi di layar konfirmasi "Yakin?"
    if (btn == "RIGHT" || btn == "OK") { // Tekan Kanan atau OK buat YES
      spamState = 1;
      isSpamming = true; // Core 0 langsung tancap gas nembak
    } 
    else if (btn == "LEFT") { // Tekan Kiri atau Back buat NO
      appMode = 0; // Balik ke menu utama
      isSpamming = false;
      aktifModeSpam = 0; // Reset ID fiturnya
    }
  } 
  else if (spamState == 1) { // Lagi di layar "RUNNING..."
    if (btn == "LEFT") { // Tekan Kiri buat STOP
      isSpamming = false;
      spamState = 0;
      appMode = 0; // Balik ke menu
      aktifModeSpam = 0;
    }
  }
}
