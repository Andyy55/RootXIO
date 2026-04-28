#include <Arduino.h>
#include <globals.h>

const unsigned char* iconListWiFi[] = {
  iconSmall_scan,    
  iconSmall_spam,    
  iconSmall_sniff,   
  iconSmall_wifi
};

const unsigned char* iconListBLE[] = {
  iconSmall_scan,    
  iconSmall_apple,   
  iconSmall_android 
};

const unsigned char* iconListIR[] = {
  iconSmall_ir,    
  iconSmall_tv,   
  iconSmall_ac,
  iconSmall_lock,
  iconSmall_saved
};

const unsigned char* iconListSet[] = {
  iconSmall_bright,    
  iconSmall_wifi,   
  iconSmall_info,
  iconSmall_repeat
};

const char* subMenuWiFi[] = {
  "Scan WiFi",
  "Beacon Spam",
  "List Scan",
  "RickRoll SSID",
};


const char* subMenuBLE[] = {
  "BLE Scanner",
  "Spam Apple",
  "Spam Android"
};


const char* subMenuIR[] = {
  "Read Signal",
  "TV B-Gone",
  "AC Remote",
  "Brute Force",
  "Saved Remotes"
};


const char* subMenuSet[] = {
  "Brightness",
  "WiFi Setup",
  "About RootX",
  "Reboot"
};



void tampilkanMenuLogo() {
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  
  if(currentMenu == 0)      display.print("#> RootX: WIFI");
  else if(currentMenu == 1) display.print("#> RootX: BLE");
  else if(currentMenu == 2) display.print("#> RootX: IR");
  else                      display.print("#> RootX: SETS");
  
  display.drawLine(0, 9, 128, 9, SSD1306_WHITE);

  const unsigned char* bigIcon;
  if(currentMenu == 0)      bigIcon = logo_wifi_32; 
  else if(currentMenu == 1) bigIcon = logo_ble_32;
  else if(currentMenu == 2) bigIcon = logo_ir_32;
  else                      bigIcon = logo_settings_32;

  display.drawBitmap(48, 22, bigIcon, 32, 32, SSD1306_WHITE);

  display.setTextSize(2); 
 
  display.setCursor(20, 30); 
  display.print("<");
  
  display.setCursor(95, 30);
  display.print(">");

  display.setTextSize(1);
  display.setCursor(40, 56);
  display.print(">SELECT<"); 
  
  display.display();
}


void tampilkanMenuUtama() { 
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  int totalSub = 0; 

  if(currentMenu == 0)      { display.print("#> RootX: WIFI"); totalSub = 4; }
  else if(currentMenu == 1) { display.print("#> RootX: BLE "); totalSub = 3; }
  else if(currentMenu == 2) { display.print("#> RootX: IR");  totalSub = 5; }
  else                      { display.print("#> RootX: SETS"); totalSub = 4; }
  display.drawLine(0, 9, 128, 9, SSD1306_WHITE);

  // --- LOOPING LIST MENU ---
  for(int i = 0; i < 5; i++) {
    int itemIndex = topMenu + i;
    if(itemIndex >= totalSub) break; 

    int yPos = 13 + (i * 10); 
    
    // Warna Teks & Kotak Kursor (Pake currentSub)
    if(itemIndex == currentSub) { 
      display.fillRect(0, yPos - 1, 128, 10, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK); 
    } else {
      display.setTextColor(SSD1306_WHITE);
    }
    
    // ========================================================
    // --- AMBIL IKON KECIL (10x10) YANG BEDA TIAP BARIS ---
    // ========================================================
    const unsigned char* iconSmall;
    if(currentMenu == 0)      iconSmall = iconListWiFi[itemIndex]; 
    else if(currentMenu == 1) iconSmall = iconListBLE[itemIndex];
    else if(currentMenu == 2) iconSmall = iconListIR[itemIndex];
    else                      iconSmall = iconListSet[itemIndex]; 

    display.drawBitmap(2, yPos - 1, iconSmall, 10, 10, (itemIndex == currentSub ? SSD1306_BLACK : SSD1306_WHITE));
    
    // TULISAN MENU
    display.setCursor(18, yPos);
    if(currentMenu == 0)      display.println(subMenuWiFi[itemIndex]);
    else if(currentMenu == 1) display.println(subMenuBLE[itemIndex]);
    else if(currentMenu == 2) display.println(subMenuIR[itemIndex]);
    else                      display.println(subMenuSet[itemIndex]);
  }
  display.display();
}

void tampilkanWifiScanner() {
  display.clearDisplay();

  // --- STATE 0: KONFIRMASI ---
  if (scannerState == 0) {
    display.fillRect(0, 0, 128, 10, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(2, 1); display.print("WIFI SCANNER");

    display.setTextColor(SSD1306_WHITE);
    display.setCursor(40, 25); display.print("Yakin??"); // Pertanyaan di tengah

    display.fillRect(0, 54, 128, 10, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(2, 55); display.print("< CANCEL");
    display.setCursor(95, 55); display.print("YES >");
  }
  
  // --- STATE 1: LOADING SCAN ---
  else if (scannerState == 1) {
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(20, 25); 
    display.print("Scanning Air..."); // Teks loading
    // Kalau scan udah beres dari Core 0, otomatis ganti ke State 2
    if (scanDone) {
      scannerState = 2; 
    }
  }
  
  // --- STATE 2: HASIL LIST (Dinamis) ---
  else if (scannerState == 2) {
  if (totalWiFi == 0) {
      display.fillRect(0, 0, 128, 10, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
      display.setCursor(2, 1); display.print("SAVED NETWORKS");

      display.setTextColor(SSD1306_WHITE);
      display.setCursor(15, 25); display.print("BELUM ADA DATA!");
      display.setCursor(10, 35); display.print("Scan WiFi dulu");

      display.fillRect(0, 54, 128, 10, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
      display.setCursor(2, 55); display.print("< BACK");
    } else {
    // HEADER
    display.fillRect(0, 0, 128, 10, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(2, 1); display.printf("SCANNER - FOUND: %d", totalWiFi);
    
    // LIST CONTENT (Highlight Blok)
    for (int i = 0; i < 3; i++) {
      int itemIdx = scrollPosScanner + i;
      if (itemIdx < totalWiFi) {
        int yPos = 14 + (i * 13);
        
        if (i == cursorInScanner) {
          display.fillRect(0, yPos - 1, 128, 12, SSD1306_WHITE);
          display.setTextColor(SSD1306_BLACK);
        } else {
          display.setTextColor(SSD1306_WHITE);
        }

        display.setCursor(1, yPos + 1); display.print(listWiFi[itemIdx].id); display.print(".");
                // ... (Kodingan print ID yang atasnya tetep sama)
        display.setCursor(1, yPos + 1); 
        display.print(listWiFi[itemIdx].id); 
        display.print(".");
        
        // ==========================================
        // LOGIKA MARQUEE (TEKS BERJALAN)
        // ==========================================
        String n = listWiFi[itemIdx].ssid;
        int maxChar = 8; // Batas aman maksimal huruf sebelum nabrak C:

        if (i == cursorInScanner && n.length() > maxChar) {
          // JIKA DI-SELECT & KEPANJANGAN -> JALANKAN ANIMASI GESER
          int kelebihan = n.length() - maxChar;
          
          // millis() / 300 artinya kecepatan geser 1 huruf per 300ms (0.3 detik)
          // + 4 itu ngasih "jeda" (pause) bentar pas teksnya udah mentok kiri
          int offset = (millis() / 300) % (kelebihan + 4); 
          
          // Kalau offset lebih dari sisa huruf, tahan di ujung (biar kebaca belakangnya)
          if (offset > kelebihan) offset = kelebihan; 
          
          display.print(n.substring(offset, offset + maxChar));
        } 
        else {
          // JIKA TIDAK DI-SELECT -> STATIS (DIPOTONG NORMAL)
          if (n.length() > maxChar) {
            display.print(n.substring(0, maxChar));
          } else {
            display.print(n);
          }
        }
        // ==========================================

        // Lanjut print Channel sama Sinyal
        display.setCursor(65, yPos + 1); display.printf("C:%d", listWiFi[itemIdx].channel);
        display.setCursor(95, yPos + 1); display.printf("%ddB", listWiFi[itemIdx].rssi);
      }
    }

    // FOOTER (Target Lock)
    display.fillRect(0, 54, 128, 10, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(2, 55); display.print("< BACK");
    
    // Teks [OK] pas di tengah (X = )
    display.setCursor(53, 55); display.print("[OK]");
  } // <--- TUTUP KURUNG STATE 2 DI SINI
}
  
   else if (scannerState == 3) {
    display.clearDisplay();
    
    // --- 1. HEADER BLOK ---
    display.fillRect(0, 0, 128, 10, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(22, 1); 
    display.print("DETAIL TARGET");

    // --- 2. BODY (INFO LENGKAP) ---
    display.setTextColor(SSD1306_WHITE);
    int xSide = 5; 
    
    // SSID Marquee
    display.setCursor(xSide, 13);
    display.print("SSID: ");
    String n = targetTerkunci.ssid;
    if (n.length() > 14) {
        int kelebihan = n.length() - 10; 
        int offset = (millis() / 250) % (kelebihan + 4);
        if (offset > kelebihan) offset = kelebihan;
        display.print(n.substring(offset, offset + 14));
    } else {
        display.print(n);
    }

    // MAC Marquee (Biar gak nimpa teks lain)
    display.setCursor(xSide, 23); 
    display.print("MAC : ");
    String m = targetTerkunci.mac;
    // MAC itu panjangnya 17, kita cuma bisa nampilin sekitar 10-11 karakter kalau ada label "MAC : "
    int maxMacChar = 10; 
    if (m.length() > maxMacChar) {
        int kelebihanM = m.length() - maxMacChar;
        int offsetM = (millis() / 300) % (kelebihanM + 4); // Speed agak beda dikit biar gak sinkron bosenin
        if (offsetM > kelebihanM) offsetM = kelebihanM;
        display.print(m.substring(offsetM, offsetM + maxMacChar));
    } else {
        display.print(m);
    }
    
    // CHANNEL
    display.setCursor(xSide, 33); 
    display.print("CH  : "); display.print(targetTerkunci.channel);
    display.print(" (2.4G)");

    // SIGNAL
    display.setCursor(xSide, 43); 
    display.print("SIG : "); display.print(targetTerkunci.rssi); 
    display.print(" dBm");

    // --- 3. FOOTER BLOK ---
    display.fillRect(0, 54, 128, 10, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(2, 55);
    display.print("[<] BACK"); 
    display.setCursor(60, 55);

    display.display();


} else if (scannerState == 4) {
    display.clearDisplay(); // Layar bersih, murni nampilin context menu

    // 1. HEADER BLOK (Putih full 128px)
    display.fillRect(0, 0, 128, 10, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(43, 1); // Pas tengah
    display.print("ACTIONS");

    // 2. BODY MENU (Bisa di-scroll)
    for(int i = 0; i < 2; i++) {
        // Karena layarnya lega, kita kasih jarak agak renggang
        int yPos = 20 + (i * 15); 
        
        uint16_t colorTheme = (contextCursor == i) ? SSD1306_BLACK : SSD1306_WHITE;
        
        // Kalau dipilih, blok dari ujung ke ujung (128px)
        if(contextCursor == i) {
            display.fillRect(0, yPos - 2, 128, 14, SSD1306_WHITE);
        }
        
        // Gambar Ikon (Agak ke tengah dikit di X: 25)
        const unsigned char* currentIcon = (i == 0) ? iconSmall_skull : iconSmall_info;
        display.drawBitmap(25, yPos, currentIcon, 10, 10, colorTheme);
        
        // Tulis Teks
        display.setTextColor(colorTheme);
        display.setCursor(45, yPos + 1);
        if (i == 0)      display.print("ATTACK");
        else             display.print("DETAILS");
    }

    // 3. FOOTER BLOK (Putih full 128px)
    display.fillRect(0, 54, 128, 10, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    
    // Kiri: < BACK
    display.setCursor(2, 55); display.print("< BACK");
    // Tengah: [OK] (X=50 biar pas di tengah layar)
    display.setCursor(53, 55); display.print("[OK]"); 
  }

  display.display();
}
void tampilkanDeauthScreen() {
  display.clearDisplay();
  
  // --- KONFIRMASI (scannerState di mode Deauth kita pinjam aja angkanya) ---
  if (deauthState == 0) {
    display.fillRect(0, 0, 128, 10, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(26, 1); display.print("DEAUTH ATTACK");
    
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 25); display.print("Attack Target?");
    display.setCursor(10, 35); display.print(targetTerkunci.ssid.substring(0,15));

    display.fillRect(0, 54, 128, 10, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(2, 55); display.print("< NO");
    display.setCursor(95, 55); display.print("YES >");
  } 
  
  // --- PROSES ATTACK ---
  else if (deauthState == 1) {
    display.fillRect(0, 0, 128, 10, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(2, 1); display.print("ATTACKING...");

    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 20); display.print("Target: "); display.print(targetTerkunci.ssid);
    display.setCursor(0, 30); display.print("Ch: "); display.print(targetTerkunci.channel);
    
    // Animasi Bar Ganti-ganti
    int bar = (millis() / 50) % 128;
    display.drawFastHLine(0, 45, bar, SSD1306_WHITE);
    
    display.fillRect(0, 54, 128, 10, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(2, 55); display.print("< STOP ATTACK");
  }

  display.display();
}


void tampilkanBrightness() {
  display.clearDisplay();

  // 1. HEADER BLOK
  display.fillRect(0, 0, 128, 10, SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(35, 1);
  display.print("BRIGHTNESS");

  // 2. BODY - Progress Bar
  display.setTextColor(SSD1306_WHITE);
  display.drawRect(14, 28, 100, 12, SSD1306_WHITE); // Frame Bar
  
  // Isi Bar (Mapping 0-255 ke lebar 96 pixel)
  int barWidth = map(brightnessValue, 0, 255, 0, 96);
  display.fillRect(16, 30, barWidth, 8, SSD1306_WHITE);

  // Persentase
  display.setCursor(55, 45);
  display.print(map(brightnessValue, 0, 255, 0, 100));
  display.print("%");

  // 3. FOOTER BLOK
  display.fillRect(0, 54, 128, 10, SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(5, 55);
  display.print("[<] BACK");
  display.setCursor(75, 55);
  display.print("[UP/DN] SET");

  display.display();
}

// Fungsi pengirim perintah kontras (taruh di bawah tampilkanBrightness)
void setOledBrightness(uint8_t level) {
  display.ssd1306_command(SSD1306_SETCONTRAST);
  display.ssd1306_command(level);
}

void tampilkanSpamScreen(String judul, String subTeks) {
  display.clearDisplay();
  
  // --- KONFIRMASI (spamState 0) ---
  if (spamState == 0) {
    display.fillRect(0, 0, 128, 10, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(2, 1); display.print(judul);
    
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 25); display.print(subTeks);

    display.fillRect(0, 54, 128, 10, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(2, 55); display.print("< NO");
    display.setCursor(95, 55); display.print("YES >");
  } 
  
  // --- PROSES SPAM (spamState 1) ---
  else if (spamState == 1) {
    display.fillRect(0, 0, 128, 10, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(2, 1); display.print("RUNNING...");

    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 25); display.print("Mode: "); display.print(subTeks);
    
    // Animasi garis biar gak kaku
    int bar = (millis() / 30) % 128;
    display.drawFastHLine(0, 45, bar, SSD1306_WHITE);
    
    display.fillRect(0, 54, 128, 10, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(2, 55); display.print("< STOP");
  }

  display.display();
}




