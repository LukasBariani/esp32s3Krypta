#include <Wire.h>
#include <Adafruit_PN532.h>
#include <DIYables_TFT_Round.h>

// --- Pin Definitions ---
#define SDA_PIN 21
#define SCL_PIN 19
#define PIN_RST 27 
#define PIN_DC 25  
#define PIN_CS 26  

// --- Colors ---
#define GREEN DIYables_TFT::colorRGB(0, 255, 0)
#define BLUE  DIYables_TFT::colorRGB(0, 0, 255)
#define WHITE DIYables_TFT::colorRGB(255, 255, 255)
#define BLACK DIYables_TFT::colorRGB(0, 0, 0)
#define RED   DIYables_TFT::colorRGB(255, 0, 0)

// --- Objects ---
Adafruit_PN532 nfc(-1, -1); // I2C Mode
DIYables_TFT_GC9A01_Round TFT_display(PIN_RST, PIN_DC, PIN_CS);

// (Optional) Include your image map data here if you want to draw it:

void showWaitingScreen() {
  TFT_display.fillScreen(BLACK);
  
  // Draw waiting graphics or text
  TFT_display.setTextColor(WHITE);
  TFT_display.setTextSize(2);
  TFT_display.setCursor(40, 70);
  TFT_display.print("AGUARDANDO");

  TFT_display.setTextColor(RED);
  TFT_display.setTextSize(1);
  TFT_display.setCursor(50, 110);
  TFT_display.print("Aproxime o cartao");
  
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("=== PN532 + Round Display (Memory Buffer Mode) ===");

  // 1. Initialize Display
  TFT_display.begin();
  TFT_display.setRotation(1);
  showWaitingScreen();

  // 2. Initialize I2C & NFC
  Wire.begin(SDA_PIN, SCL_PIN);
  delay(100);

  uint32_t version = nfc.getFirmwareVersion();
  if (!version) {
    Serial.println("PN532 NAO ENCONTRADO!");
    TFT_display.fillScreen(BLACK);
    TFT_display.setTextColor(RED);
    TFT_display.setCursor(30, 110);
    TFT_display.print("Erro: PN532");
    while (1) { delay(1000); }
  }

  nfc.SAMConfig();
  Serial.println("PN532 pronto!");
}

void loop() {
  uint8_t uid[7] = {0};
  uint8_t uidLength = 0;

  // Check quickly if an NFC card is present (50ms timeout so the loop stays responsive)
  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 50)) {
    // --- CARD DETECTED ---
    String uidStr = "";
    for (uint8_t i = 0; i < uidLength; i++) {
      if (uid[i] < 0x10) uidStr += "0";
      uidStr += String(uid[i], HEX);
      if (i < uidLength - 1) uidStr += ":";
    }
    uidStr.toUpperCase();
    Serial.print("Tag Detectada! UID: ");
    Serial.println(uidStr);

    // Show Success Screen on Display
    TFT_display.fillScreen(BLACK);
    TFT_display.setTextColor(GREEN);
    TFT_display.setTextSize(2);
    TFT_display.setCursor(35, 70);
    TFT_display.print("Card OK!");

    TFT_display.setTextColor(WHITE);
    TFT_display.setTextSize(1);
    TFT_display.setCursor(30, 120);
    TFT_display.print("UID:");

    TFT_display.setTextColor(BLUE);
    TFT_display.setCursor(30, 140);
    TFT_display.print(uidStr);

    // Keep success screen visible for 3 seconds before resetting
    delay(3000);

    // Reset back to waiting screen
    showWaitingScreen();

  } else {
    // --- NO CARD: KEEP WAITING ---
    // You can add animation frame updates here if you have multiple array maps!
    delay(100);
  }
}