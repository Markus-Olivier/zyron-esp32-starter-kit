/*
 * 13 — REST API Client
 * Zyron ESP32 Starter Kit | https://zyron.co.za
 *
 * POST sensor data as a JSON payload to any REST API endpoint.
 * Works with custom backends, Firebase, ThingSpeak, or any HTTP API.
 *
 * Wiring:
 *   DHT22 DATA   → GPIO 4  (+ 10kΩ pull-up)
 *   HC-SR04 TRIG → GPIO 5
 *   HC-SR04 ECHO → GPIO 18 (voltage divider!)
 *   PIR OUT      → GPIO 13
 *
 * Required Libraries:
 *   - DHT sensor library (Adafruit)
 *   - ArduinoJson
 *
 * Skills: HTTPClient, JSON serialisation, REST API integration, error handling
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

// ── Configuration ─────────────────────────────────────────────────────────────
const char* WIFI_SSID    = "YOUR_WIFI_SSID";
const char* WIFI_PASS    = "YOUR_WIFI_PASSWORD";
const char* API_ENDPOINT = "https://your-api.example.com/sensors"; // Change this
const char* API_KEY      = "YOUR_API_KEY"; // Add to X-API-Key header (optional)

const uint32_t POST_INTERVAL_MS = 10000; // Post every 10 seconds
// ──────────────────────────────────────────────────────────────────────────────

#define DHT_PIN   4
#define DHT_TYPE  DHT22
#define TRIG_PIN  5
#define ECHO_PIN  18
#define PIR_PIN   13

DHT dht(DHT_PIN, DHT_TYPE);
uint32_t lastPost = 0;

float measureDistanceCm() {
    digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    long dur = pulseIn(ECHO_PIN, HIGH, 30000UL);
    return (dur == 0) ? -1.0f : dur * 0.034f / 2.0f;
}

bool postSensorData() {
    float temp  = dht.readTemperature();
    float hum   = dht.readHumidity();
    float dist  = measureDistanceCm();
    bool  motion = digitalRead(PIR_PIN) == HIGH;

    // Build JSON payload
    StaticJsonDocument<256> doc;
    doc["device_id"]     = "zyron-esp32-001";
    doc["temperature_c"] = isnan(temp) ? nullptr : serialized(String(temp, 2));
    doc["humidity_pct"]  = isnan(hum)  ? nullptr : serialized(String(hum, 2));
    doc["distance_cm"]   = dist;
    doc["motion"]        = motion;
    doc["uptime_s"]      = millis() / 1000;

    String body;
    serializeJson(doc, body);

    HTTPClient http;
    http.begin(API_ENDPOINT);
    http.addHeader("Content-Type", "application/json");
    if (strlen(API_KEY) > 0) {
        http.addHeader("X-API-Key", API_KEY);
    }
    http.setTimeout(10000);

    Serial.printf("POST %s\nBody: %s\n", API_ENDPOINT, body.c_str());
    int code = http.POST(body);

    if (code > 0) {
        Serial.printf("Response: %d — %s\n", code, http.getString().c_str());
        http.end();
        return code >= 200 && code < 300;
    } else {
        Serial.printf("POST failed: %s\n", http.errorToString(code).c_str());
        http.end();
        return false;
    }
}

void setup() {
    Serial.begin(115200);
    dht.begin();
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(PIR_PIN,  INPUT);
    digitalWrite(TRIG_PIN, LOW);

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
    Serial.printf("\nConnected. IP: %s\n", WiFi.localIP().toString().c_str());
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi lost, reconnecting...");
        WiFi.reconnect();
        delay(5000);
        return;
    }

    if (millis() - lastPost >= POST_INTERVAL_MS) {
        lastPost = millis();
        bool ok = postSensorData();
        Serial.printf("POST %s\n\n", ok ? "succeeded" : "failed");
    }
}
