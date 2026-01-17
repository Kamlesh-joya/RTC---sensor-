#include <WiFi.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WebServer.h>
#include <RTClib.h>

/* ========= PIN ========= */
#define SDA_PIN 21
#define SCL_PIN 22

/* ========= LCD ========= */
LiquidCrystal_I2C lcd(0x27, 20, 4);

/* ========= RTC ========= */
RTC_DS3231 rtc;

/* ========= WIFI ========= */
const char* ssid = "Excitel_HARSHIT_2.4G";
const char* password = "dinesh480tak";

/* ========= WEB ========= */
WebServer server(80);

/* ========= VAR ========= */
bool otaEnabled = false;
String timeStr = "--:--:--";
String dateStr = "--/--/----";

/* ========= WEB PAGE ========= */
void handleRoot() {
  String html = "<h2>ESP32 PRACTICAL STATUS</h2>";
  html += "<p><b>IP:</b> " + WiFi.localIP().toString() + "</p>";
  html += "<p><b>WiFi:</b> ";
  html += (WiFi.status() == WL_CONNECTED) ? "Connected</p>" : "Disconnected</p>";
  html += "<p><b>OTA:</b> ";
  html += otaEnabled ? "ON</p>" : "OFF</p>";
  html += "<p><b>Practical:</b> RTC</p>";
  html += "<p><b>Time:</b> " + timeStr + "</p>";
  html += "<p><b>Date:</b> " + dateStr + "</p>";

  server.send(200, "text/html", html);
}

/* ================= SETUP ================= */
void setup() {
  Serial.begin(115200);

  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.init();
  lcd.backlight();

  /* ---- RTC ---- */
  if (!rtc.begin()) {
    Serial.println("RTC not found");
  }

  // Uncomment ONLY ONCE to set time
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  /* ---- WIFI ---- */
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(ssid, password);

  lcd.setCursor(0,0);
  lcd.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
  }

  /* ---- OTA ---- */
  ArduinoOTA.setHostname("ESP32-RTC");
  ArduinoOTA.begin();
  otaEnabled = true;

  /* ---- WEB ---- */
  server.on("/", handleRoot);
  server.begin();
}

/* ================= LOOP ================= */
void loop() {
  ArduinoOTA.handle();
  server.handleClient();

  readRTC();
  updateLCD();
}

/* ================= RTC READ ================= */
void readRTC() {
  static unsigned long lastRTC = 0;
  if (millis() - lastRTC < 1000) return;
  lastRTC = millis();

  DateTime now = rtc.now();

  char tBuf[9];
  sprintf(tBuf, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  timeStr = String(tBuf);

  char dBuf[11];
  sprintf(dBuf, "%02d-%02d-%04d", now.day(), now.month(), now.year());
  dateStr = String(dBuf);
}

/* ================= LCD UPDATE ================= */
void updateLCD() {
  static unsigned long lastLCD = 0;
  if (millis() - lastLCD < 1000) return;
  lastLCD = millis();

  lcd.clear();

  // Row 1: IP
  lcd.setCursor(0,0);
  lcd.print("IP:");
  lcd.print(WiFi.localIP());

  // Row 2: OTA + WiFi
  lcd.setCursor(0,1);
  lcd.print("OTA:");
  lcd.print(otaEnabled ? "ON " : "OFF");
  lcd.print(" WiFi:");
  lcd.print(WiFi.status() == WL_CONNECTED ? "ON" : "OFF");

  // Row 3: Practical Name
  lcd.setCursor(0,2);
  lcd.print("Practical:RTC");

  // Row 4: Time & Date
  lcd.setCursor(0,3);
  lcd.print(timeStr);
  lcd.print(" ");
  lcd.print(dateStr.substring(0, 10));
}


