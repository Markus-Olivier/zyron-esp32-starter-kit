/*
 * 12 — MQTT Publisher
 * Zyron ESP32 Starter Kit | https://zyron.co.za
 *
 * Publish sensor data to an MQTT broker (local Mosquitto, HiveMQ,
 * Home Assistant, or any MQTT cloud broker). Each sensor has its own
 * topic so subscribers can listen to individual values.
 *
 * Wiring:
 *   DHT22 DATA   → GPIO 4  (+ 10kΩ pull-up)
 *   HC-SR04 TRIG → GPIO 5
 *   HC-SR04 ECHO → GPIO 18 (voltage divider!)
 *   PIR OUT      → GPIO 13
 *
 * MQTT Topics published:
 *   zyron/esp32/temperature
 *   zyron/esp32/humidity
 *   zyron/esp32/distance
 *   zyron/esp32/motion
 *   zyron/esp32/status
 *
 * Required Libraries:
 *   - PubSubClient (Nick O'Leary)
 *   - DHT sensor library (Adafruit)
 *   - ArduinoJson
 *
 * Skills: MQTT publish/subscribe, reconnect logic, JSON payloads
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

// ── Configuration ─────────────────────────────────────────────────────────────
const char* WIFI_SSID   = "YOUR_WIFI_SSID";
const char* WIFI_PASS   = "YOUR_WIFI_PASSWORD";
const char* MQTT_SERVER = "192.168.1.100";   // Your broker IP
const int   MQTT_PORT   = 1883;
const char* MQTT_USER   = "";                // Leave empty for local brokers with no auth
const char* MQTT_PASS   = "";                // WARNING: add credentials + TLS for cloud/public brokers
const char* CLIENT_ID   = "zyron-esp32-001";

const uint32_t PUBLISH_INTERVAL_MS = 5000;
// ──────────────────────────────────────────────────────────────────────────────

#define DHT_PIN   4
#define DHT_TYPE  DHT22
#define TRIG_PIN  5
#define ECHO_PIN  18
#define PIR_PIN   13

DHT         dht(DHT_PIN, DHT_TYPE);
WiFiClient  wifiClient;
PubSubClient mqtt(wifiClient);

uint32_t lastPublish = 0;

float measureDistanceCm() {
    digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    long dur = pulseIn(ECHO_PIN, HIGH, 30000UL);
    return (dur == 0) ? -1.0f : dur * 0.034f / 2.0f;
}

void connectWiFi() {
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("WiFi");
    while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
    Serial.printf(" connected. IP: %s\n", WiFi.localIP().toString().c_str());
}

void connectMQTT() {
    while (!mqtt.connected()) {
        Serial.printf("Connecting to MQTT broker %s...", MQTT_SERVER);
        bool ok = (strlen(MQTT_USER) > 0)
            ? mqtt.connect(CLIENT_ID, MQTT_USER, MQTT_PASS, "zyron/esp32/status", 0, true, "offline")
            : mqtt.connect(CLIENT_ID, nullptr, nullptr, "zyron/esp32/status", 0, true, "offline");

        if (ok) {
            Serial.println(" connected.");
            mqtt.publish("zyron/esp32/status", "online", true);
        } else {
            Serial.printf(" failed (rc=%d). Retry in 5s.\n", mqtt.state());
            delay(5000);
        }
    }
}

void publishSensors() {
    float temp  = dht.readTemperature();
    float hum   = dht.readHumidity();
    float dist  = measureDistanceCm();
    bool  motion = digitalRead(PIR_PIN) == HIGH;

    char buf[16];

    if (!isnan(temp)) {
        dtostrf(temp, 4, 1, buf);
        mqtt.publish("zyron/esp32/temperature", buf, false);
    }
    if (!isnan(hum)) {
        dtostrf(hum, 4, 1, buf);
        mqtt.publish("zyron/esp32/humidity", buf, false);
    }
    if (dist > 0) {
        dtostrf(dist, 5, 1, buf);
        mqtt.publish("zyron/esp32/distance", buf, false);
    }
    mqtt.publish("zyron/esp32/motion", motion ? "true" : "false", false);

    // Also publish a combined JSON payload
    StaticJsonDocument<200> doc;
    doc["temp_c"]    = isnan(temp) ? nullptr : serialized(String(temp, 1));
    doc["hum_pct"]   = isnan(hum)  ? nullptr : serialized(String(hum, 1));
    doc["dist_cm"]   = dist;
    doc["motion"]    = motion;
    doc["uptime_s"]  = millis() / 1000;

    String json;
    serializeJson(doc, json);
    mqtt.publish("zyron/esp32/json", json.c_str(), false);

    Serial.printf("Published: temp=%.1f hum=%.1f dist=%.1f motion=%d\n",
                  temp, hum, dist, motion);
}

void setup() {
    Serial.begin(115200);
    dht.begin();
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(PIR_PIN,  INPUT);
    digitalWrite(TRIG_PIN, LOW);

    connectWiFi();
    mqtt.setServer(MQTT_SERVER, MQTT_PORT);
    mqtt.setKeepAlive(60);
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) connectWiFi();
    if (!mqtt.connected()) connectMQTT();
    mqtt.loop();

    if (millis() - lastPublish >= PUBLISH_INTERVAL_MS) {
        lastPublish = millis();
        publishSensors();
    }
}
