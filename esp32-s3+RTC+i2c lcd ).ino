#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <time.h>

/* ---------- WIFI DETAILS ---------- */
const char* ssid = "ESP-404";
const char* password = "k756756756";

/* ---------- I2C PINS ---------- */
#define SDA_PIN 8
#define SCL_PIN 9

/* ---------- LCD ---------- */
LiquidCrystal_I2C lcd(0x27, 16, 2);

/* ---------- RTC ---------- */
RTC_DS3231 rtc;

/* ---------- TIMEZONE (IST) ---------- */
const long gmtOffset_sec = 5 * 3600 + 30 * 60;
const int daylightOffset_sec = 0;

/* ---------- DAYS ---------- */
char daysOfWeek[7][12] = {
  "Sunday", "Monday", "Tuesday",
  "Wednesday", "Thursday", "Friday", "Saturday"
};

void setup() {
  Serial.begin(115200);

  /* ---------- I2C INIT ---------- */
  Wire.begin(SDA_PIN, SCL_PIN);

  /* ---------- LCD INIT ---------- */
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("RTC + WiFi");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");
  delay(2000);
  lcd.clear();

  /* ---------- RTC INIT ---------- */
  if (!rtc.begin()) {
    lcd.print("RTC NOT FOUND");
    while (1);
  }

  /* ---------- WIFI CONNECT ---------- */
  lcd.print("Connecting WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  lcd.clear();
  lcd.print("WiFi Connected");
  delay(1000);

  /* ---------- NTP TIME ---------- */
  configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org");

  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    rtc.adjust(DateTime(
      timeinfo.tm_year + 1900,
      timeinfo.tm_mon + 1,
      timeinfo.tm_mday,
      timeinfo.tm_hour,
      timeinfo.tm_min,
      timeinfo.tm_sec
    ));

    lcd.clear();
    lcd.print("Time Synced");
    delay(1500);
  } else {
    lcd.clear();
    lcd.print("NTP Failed");
    delay(2000);
  }

  /* ---------- WIFI OFF ---------- */
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  lcd.clear();
}

void loop() {
  DateTime now = rtc.now();

  /* ---------- LINE 1: DAY ---------- */
  lcd.setCursor(0, 0);
  lcd.print(daysOfWeek[now.dayOfTheWeek()]);
  lcd.print("     ");

  /* ---------- LINE 2: DATE + TIME (12H) ---------- */
  lcd.setCursor(0, 1);

  // Date
  if (now.day() < 10) lcd.print("0");
  lcd.print(now.day());
  lcd.print("/");

  if (now.month() < 10) lcd.print("0");
  lcd.print(now.month());
  lcd.print("/");

  lcd.print(now.year());
  lcd.print(" ");

  // -------- 12-HOUR FORMAT --------
  int hour12;
  String ampm;

  if (now.hour() == 0) {
    hour12 = 12;
    ampm = "AM";
  } else if (now.hour() < 12) {
    hour12 = now.hour();
    ampm = "AM";
  } else if (now.hour() == 12) {
    hour12 = 12;
    ampm = "PM";
  } else {
    hour12 = now.hour() - 12;
    ampm = "PM";
  }

  if (hour12 < 10) lcd.print("0");
  lcd.print(hour12);
  lcd.print(":");

  if (now.minute() < 10) lcd.print("0");
  lcd.print(now.minute());
  lcd.print(":");

  if (now.second() < 10) lcd.print("0");
  lcd.print(now.second());

  lcd.print(" ");
  lcd.print(ampm);

  delay(1000);
}



