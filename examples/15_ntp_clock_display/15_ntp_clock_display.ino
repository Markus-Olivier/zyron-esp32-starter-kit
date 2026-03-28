/*
 * 15 — NTP Clock with OLED Display
 * Zyron ESP32 Starter Kit | https://zyron.co.za
 *
 * Sync time from an NTP server over WiFi and display it on the OLED.
 * Shows current time, date, and uptime. No RTC module needed — the
 * ESP32 keeps time in software using the internal timer.
 *
 * Wiring:
 *   OLED VCC → 3.3V
 *   OLED GND → GND
 *   OLED SCL → GPIO 22
 *   OLED SDA → GPIO 21
 *
 * Required Libraries:
 *   - Adafruit SSD1306
 *   - Adafruit GFX Library
 *
 * Skills: NTP, configTime, struct tm, OLED large text, timezone offset
 */

#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "time.h"

// ── Configuration ─────────────────────────────────────────────────────────────
const char* WIFI_SSID  = "YOUR_WIFI_SSID";
const char* WIFI_PASS  = "YOUR_WIFI_PASSWORD";
const char* NTP_SERVER = "pool.ntp.org";
const long  UTC_OFFSET = 7200;    // South Africa Standard Time: UTC+2 = 7200 s
const int   DST_OFFSET = 0;       // South Africa does not observe DST
// ──────────────────────────────────────────────────────────────────────────────

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT  64
#define OLED_RESET     -1
#define OLED_ADDRESS  0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void displayClock() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        display.clearDisplay();
        display.setCursor(0, 20);
        display.setTextSize(1);
        display.println("Waiting for NTP...");
        display.display();
        return;
    }

    char timeBuf[10], dateBuf[20];
    strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", &timeinfo);
    strftime(dateBuf, sizeof(dateBuf), "%a %d %b %Y",  &timeinfo);

    display.clearDisplay();

    // Large clock in the middle
    display.setTextSize(2);
    display.setCursor(4, 10);
    display.println(timeBuf);

    // Date below
    display.setTextSize(1);
    display.setCursor(4, 38);
    display.println(dateBuf);

    // Uptime footer
    display.setCursor(4, 54);
    display.printf("up %lus  zyron.co.za", millis() / 1000);

    display.display();
}

void setup() {
    Serial.begin(115200);

    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
        Serial.println("[ERROR] OLED not found.");
        while (true);
    }
    display.setTextColor(SSD1306_WHITE);
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Connecting WiFi...");
    display.display();

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) { delay(500); }
    Serial.printf("Connected. IP: %s\n", WiFi.localIP().toString().c_str());

    configTime(UTC_OFFSET, DST_OFFSET, NTP_SERVER);
    Serial.println("Syncing NTP time...");

    // Wait for time to sync
    struct tm t;
    int attempts = 0;
    while (!getLocalTime(&t) && attempts++ < 20) { delay(500); }

    if (attempts >= 20) {
        Serial.println("[WARN] NTP sync timed out — time may be wrong.");
    } else {
        char buf[32];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &t);
        Serial.printf("Time synced: %s (UTC+%ld)\n", buf, UTC_OFFSET / 3600);
    }
}

void loop() {
    displayClock();
    delay(1000);
}
