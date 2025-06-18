#define BLYNK_TEMPLATE_ID "TMPL6PJEDLLXx"
#define BLYNK_TEMPLATE_NAME "Humidifier"
#define BLYNK_AUTH_TOKEN "WizPCfkBq07EVvxT-_NP3X12bZr0dTNC"

#include <Arduino.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <WiFiManager.h>

// ====== Pin Configuration ======
#define DHTPIN 4                // Pin untuk sensor DHT22
#define DHTTYPE DHT22           // Tipe sensor
#define RELAY_PIN 5             // Pin relay mist maker
#define SDA_PIN 21              // SDA untuk I2C
#define SCL_PIN 22              // SCL untuk I2C

// ====== Virtual Pin Configuration untuk Blynk ======
#define BLYNK_MODE V0
#define BLYNK_MANUAL_SWITCH V1
#define BLYNK_TEMP V2
#define BLYNK_HUM V3
#define BLYNK_STATUS V4
#define BLYNK_RESET_WIFI V5

// ====== Global Variables ======
DHT dht(DHTPIN, DHTTYPE);                   // Inisialisasi sensor DHT
LiquidCrystal_I2C lcd(0x27, 16, 2);         // Inisialisasi LCD 16x2 dengan alamat 0x27

bool isManualMode = false;                 // Status mode manual
bool manualControl = false;                // Status kontrol manual
bool mistOn = false;                       // Status mist maker
unsigned long lastUpdate = 0;              // Waktu update terakhir

unsigned long manualStartTime = 0;         // Waktu mulai mode manual
const unsigned long MANUAL_TIMEOUT = 30 * 60 * 1000; // Timeout 30 menit

// ====== Fungsi untuk update status mist ke Blynk ======
void updateMistStatus() {
  String status = mistOn ? "ON" : "OFF";
  Blynk.virtualWrite(BLYNK_STATUS, status);
}

// ====== Fungsi update status mode ke Blynk ======
void updateBlynkModeStatus() {
  Blynk.virtualWrite(BLYNK_MODE, isManualMode ? 1 : 0);
}

// ====== Fungsi update tampilan LCD ======
void updateLCD(float t, float h) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:" + String(t, 1) + "C");
  lcd.setCursor(9, 0);
  lcd.print("H:" + String(h, 0) + "%");

  lcd.setCursor(0, 1);
  lcd.print("M:");
  lcd.print(isManualMode ? "Manual" : "Auto  ");
  lcd.setCursor(9, 1);
  lcd.print("S:");
  lcd.print(mistOn ? "ON " : "OFF");

  updateMistStatus();
}

// ====== Fungsi kontrol mist maker ======
void updateMistControl(float h) {
  if (isManualMode) {
    if (manualControl) {
      if (!mistOn) {
        digitalWrite(RELAY_PIN, HIGH);
        mistOn = true;
        manualStartTime = millis();
      } else if (millis() - manualStartTime > MANUAL_TIMEOUT) {
        digitalWrite(RELAY_PIN, LOW);
        mistOn = false;
        manualControl = false;
        Blynk.virtualWrite(BLYNK_MANUAL_SWITCH, 0);
      }
    } else if (mistOn) {
      digitalWrite(RELAY_PIN, LOW);
      mistOn = false;
    }
  } else {
    if (h < 60.0 && !mistOn) {
      digitalWrite(RELAY_PIN, HIGH);
      mistOn = true;
      Serial.println("Mist ON (Auto - Hum < 60%)");
    } else if (h >= 70.0 && mistOn) {
      digitalWrite(RELAY_PIN, LOW);
      mistOn = false;
      Serial.println("Mist OFF (Auto - Hum >= 70%)");
    }
  }

  updateMistStatus();
}

// ====== Fungsi callback ketika Blynk terhubung ======
BLYNK_CONNECTED() {
  Blynk.syncVirtual(BLYNK_MODE);
  Blynk.syncVirtual(BLYNK_MANUAL_SWITCH);
  Blynk.syncVirtual(BLYNK_RESET_WIFI);
  updateBlynkModeStatus();
}

// ====== Fungsi untuk menangani perubahan mode manual/auto ======
BLYNK_WRITE(BLYNK_MODE) {
  isManualMode = param.asInt();
  Serial.println(isManualMode ? "Manual Mode" : "Auto Mode");
  if (!isManualMode) {
    manualControl = false;
    Blynk.virtualWrite(BLYNK_MANUAL_SWITCH, 0);
  }
  updateBlynkModeStatus();
}

// ====== Fungsi untuk menangani kontrol manual dari aplikasi ======
BLYNK_WRITE(BLYNK_MANUAL_SWITCH) {
  manualControl = param.asInt();
  Serial.print("Manual switch changed to: ");
  Serial.println(manualControl);
  manualStartTime = millis();
}

// ====== Fungsi untuk reset pengaturan WiFi ======
BLYNK_WRITE(BLYNK_RESET_WIFI) {
  if (param.asInt() == 1) {
    Serial.println("Resetting WiFi settings...");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Reset WiFi...");
    delay(1000);

    WiFiManager wm;
    wm.resetSettings();
    ESP.restart();
  }
}

// ====== Setup awal perangkat ======
void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.init();
  lcd.backlight();

  dht.begin();
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  lcd.setCursor(0, 0);
  lcd.print("WiFi Config...");

  WiFiManager wm;
  bool res = wm.autoConnect("Humidifier-Setup", "12345678");

  if (!res) {
    Serial.println("Failed to connect & no config saved");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Failed :(");
    delay(3000);
    ESP.restart();
  }

  Serial.println("WiFi Connected!");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi OK!");
  delay(1000);

  Blynk.begin(BLYNK_AUTH_TOKEN, WiFi.SSID().c_str(), WiFi.psk().c_str());
  lcd.setCursor(0, 1);
  lcd.print("Blynk Connect..");
}

// ====== Loop utama ======
void loop() {
  Blynk.run();

  unsigned long currentMillis = millis();
  if (currentMillis - lastUpdate >= 2000) {
    lastUpdate = currentMillis;

    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (isnan(t) || isnan(h)) {
      Serial.println("Sensor Error");
      lcd.setCursor(0, 0);
      lcd.print("Sensor Error   ");
      lcd.setCursor(0, 1);
      lcd.print("Check DHT22     ");
      Blynk.virtualWrite(BLYNK_TEMP, "Err");
      Blynk.virtualWrite(BLYNK_HUM, "Err");
      return;
    }

    Serial.printf("Temp: %.2f C | Hum: %.2f %%\n", t, h);
    updateLCD(t, h);
    Blynk.virtualWrite(BLYNK_TEMP, t);
    Blynk.virtualWrite(BLYNK_HUM, h);

    updateMistControl(h);
  }
}
