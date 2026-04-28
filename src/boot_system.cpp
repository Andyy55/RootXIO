#include <Arduino.h>
#include <globals.h>


// 1. Fungsi Tampilan Logo Saja
void tampilkanLogoDulu() {
  display.clearDisplay();
  // Gambar logo mahkota/foto lu dari photo_data.h
  display.drawBitmap(0, 0, my_photo_bmp, 128, 64, SSD1306_WHITE);
  display.display();
  delay(1000); // Logo nampil detik
}

void tampilkanIntroAnime() {
  // 1. EFEK GLITCH KILAT (0.2 - 0.3 detik)
  for (int i = 0; i < 8; i++) {
    display.clearDisplay();
    // Gambar garis-garis acak biar kayak layar rusak/glitch
    for (int j = 0; j < 10; j++) {
      int y = random(0, 64);
      display.drawFastHLine(0, y, 128, SSD1306_WHITE);
    }
    display.display();
    delay(30); // Kecepatan kedip glitch
  }

  // 2. TAMPILAN STATIC (FOTO + TEKS)
  display.clearDisplay();
  
  // Gambar Foto Anime di Kiri (0,0, 64x64)
  // Pastiin nama variabel foto anime lu di photo_data.h itu 'foto_anime_64'
  display.drawBitmap(0, 0, foto_anime_64, 64, 64, SSD1306_WHITE);
  
  // Garis Pemisah (Opsional tapi biar rapi)
  display.drawFastVLine(65, 0, 64, SSD1306_WHITE);

  // Teks Firmware di Kanan (Center secara vertikal)
    // Teks Firmware di Kanan (Mulai dari x=68 biar ada gap dikit dari tengah)
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  // Baris 1: Judul (Agak dikasih jarak atas)
  display.setCursor(68, 6);  display.print("[FIRMWARE]");
  
  // Baris 2-6: Info Detail
  display.setCursor(67, 18); display.print("Name:RootX");
  display.setCursor(67, 28); display.print("Ver :1.0.0");
  display.setCursor(67, 38); display.print("By  :Andyy");
  display.setCursor(67, 48); display.print("Mode:GOD");
  display.setCursor(67, 56); display.print("Stat:Opt"); 

  display.display();
  delay(2500); // Tahan 3 detik biar orang bisa liat Waifu & Nama lu wkwk
}

// 2. Fungsi Teks Splash (Setelah Logo Hilang)
void tampilkanTeksSplash() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);

  // Efek ngetik teks hacker
  String baris1 = ">> Initializing...";
  String baris2 = ">> Checking Flash...";
  String baris3 = ">> Checking PSRAM...";
  String baris4 = ">> ROOTX READY!!";

  ketikTeks(baris1, 0, 10);
  delay(150);
  ketikTeks(baris2, 0, 22);
  delay(150);
  ketikTeks(baris3, 0, 34);
  delay(150);
  ketikTeks(baris4, 0, 46);
  
  delay(800); // Teks nampil 2 detik sebelum masuk menu
}

// Fungsi bantu buat efek ngetik per baris
void ketikTeks(String teks, int x, int y) {
  display.setCursor(x, y);
  for (int i = 0; i < teks.length(); i++) {
    display.print(teks[i]);
    display.display();
    delay(25); 
  }
  display.println();
}
