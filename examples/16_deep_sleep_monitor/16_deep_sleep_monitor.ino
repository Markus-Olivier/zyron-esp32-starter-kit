/*
 * 16 — Deep Sleep Battery Monitor
 * Zyron ESP32 Starter Kit | https://zyron.co.za
 *
 * Ultra-low power monitoring: the ESP32 wakes from deep sleep,
 * reads sensors, posts data, then goes back to sleep. Ideal for
 * battery-powered remote sensors.
 *
 * Wake sources:
 *   1. Timer — every SLEEP_DURATION_S seconds
 *   2. PIR motion — ext0 wakeup on GPIO 13 HIGH
 *
 * Power consumption comparison:
 *   Active WiFi:    ~240 mA
 *   Modem sleep:    ~15 mA
 *   Light sleep:    ~0.8 mA
 *   Deep sleep:     ~10 µA  ← this sketch
 *
 * Wiring:
 *   DHT22 DATA   → GPIO 4
 *   PIR OUT      → GPIO 13  (also acts as ext0 wakeup)
 *
 * Required Libraries:
 *   - DHT sensor library (Adafruit)
 *   - ArduinoJson
 *
 * Skills: deep sleep, RTC memory, wake reason, power optimisation
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include "esp_sleep.h"

// ── Configuration ─────────────────────────────────────────────────────────────
const char* WIFI_SSID    = "YOUR_WIFI_SSID";
const char* WIFI_PASS    = "YOUR_WIFI_PASSWORD";
const char* API_ENDPOINT = "https://your-api.example.com/sensors";

const uint64_t SLEEP_DURATION_S = 60; // Wake every 60 seconds for timed reading
// ──────────────────────────────────────────────────────────────────────────────

#define DHT_PIN  4
#define DHT_TYPE DHT22
#define PIR_PIN  13

// RTC memory persists across deep sleep cycles
RTC_DATA_ATTR int    bootCount    = 0;
RTC_DATA_ATTR float  lastTempC    = 0;
RTC_DATA_ATTR float  lastHumidity = 0;

DHT dht(DHT_PIN, DHT_TYPE);

String wakeReasonStr() {
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    switch (cause) {
        case ESP_SLEEP_WAKEUP_EXT0:   return "PIR motion (EXT0)";
        case ESP_SLEEP_WAKEUP_TIMER:  return "Timer";
        default:                      return "Power-on / reset";
    }
}

bool postData(float temp, float hum, const String& wakeReason) {
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts++ < 20) { delay(500); }
    if (WiFi.status() != WL_CONNECTED) return false;

    StaticJsonDocument<256> doc;
    doc["device_id"]     = "zyron-esp32-001";
    doc["temperature_c"] = temp;
    doc["humidity_pct"]  = hum;
    doc["wake_reason"]   = wakeReason;
    doc["boot_count"]    = bootCount;

    String body;
    serializeJson(doc, body);

    HTTPClient http;
    http.begin(API_ENDPOINT);
    http.addHeader("Content-Type", "application/json");
    int code = http.POST(body);
    http.end();
    WiFi.disconnect(true);

    return code >= 200 && code < 300;
}

void goToSleep() {
    // Configure wakeup sources
    esp_sleep_enable_timer_wakeup(SLEEP_DURATION_S * 1000000ULL);
    esp_sleep_enable_ext0_wakeup((gpio_num_t)PIR_PIN, HIGH);

    Serial.printf("Entering deep sleep for %llu seconds (or until PIR motion)...\n",
                  SLEEP_DURATION_S);
    Serial.flush();
    esp_deep_sleep_start();
    // Execution never reaches here — ESP32 restarts from setup() on wake
}

void setup() {
    Serial.begin(115200);
    bootCount++;

    String wakeReason = wakeReasonStr();
    Serial.printf("\n=== Boot #%d | Wake: %s ===\n", bootCount, wakeReason.c_str());

    dht.begin();
    delay(2000); // DHT22 stabilisation

    float temp = dht.readTemperature();
    float hum  = dht.readHumidity();

    if (isnan(temp) || isnan(hum)) {
        Serial.println("[WARN] DHT22 read failed — using last known values.");
        temp = lastTempC;
        hum  = lastHumidity;
    } else {
        lastTempC    = temp;
        lastHumidity = hum;
    }

    Serial.printf("Temp: %.1f°C  Hum: %.1f%%\n", temp, hum);

    bool ok = postData(temp, hum, wakeReason);
    Serial.printf("POST: %s\n", ok ? "OK" : "FAILED");

    goToSleep();
}

void loop() {
    // Never runs — deep sleep restarts from setup()
}
