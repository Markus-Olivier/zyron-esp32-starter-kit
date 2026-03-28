/*
 * 20 — Full Weather Station
 * Zyron ESP32 Starter Kit | https://zyron.co.za
 *
 * The capstone project: a complete WiFi weather station with:
 *   - DHT22  → temperature, humidity, heat index, dew point
 *   - LDR    → UV/ambient light index
 *   - OLED   → rotating local display (clock, climate, light)
 *   - NTP    → real-time clock (no RTC module needed)
 *   - JSON API → GET /data returns all readings
 *   - Web UI  → live dashboard at http://<ip>/
 *   - MQTT   → publishes to Home Assistant / Node-RED
 *   - OTA    → update firmware wirelessly
 *
 * Wiring:
 *   DHT22 DATA   → GPIO 4  (+ 10kΩ pull-up)
 *   LDR          → GPIO 34 (voltage divider)
 *   OLED SCL     → GPIO 22
 *   OLED SDA     → GPIO 21
 *
 * Required Libraries:
 *   - Adafruit SSD1306 + GFX, DHT sensor library, PubSubClient, ArduinoJson, ArduinoOTA
 *
 * Skills: system integration, multi-task loop, state machine, full-stack IoT
 */

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "time.h"

// ── Configuration ─────────────────────────────────────────────────────────────
const char* WIFI_SSID   = "YOUR_WIFI_SSID";
const char* WIFI_PASS   = "YOUR_WIFI_PASSWORD";
const char* MQTT_SERVER = "192.168.1.100"; // Your local broker IP
const int   MQTT_PORT   = 1883;             // Use 8883 + TLS for cloud/public brokers
const char* DEVICE_ID   = "zyron-weather-01"; // Change per device to avoid topic collisions
const long  UTC_OFFSET  = 7200;  // UTC+2 (SAST)
const uint32_t PUBLISH_INTERVAL = 30000;
// ──────────────────────────────────────────────────────────────────────────────

#define DHT_PIN  4
#define LDR_PIN  34
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT  64
#define OLED_RESET -1
#define OLED_ADDR  0x3C

DHT          dht(DHT_PIN, DHT22);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
WiFiClient   wifiClient;
PubSubClient mqtt(wifiClient);
WebServer    server(80);

// Readings
float  tempC = 0, tempF = 0, humidity = 0, heatIndex = 0, dewPoint = 0;
int    ldrRaw = 0;
uint32_t lastPublish = 0;
uint8_t  displayPage = 0;
uint32_t lastPageSwitch = 0;

// ── Sensor read ───────────────────────────────────────────────────────────────
float calcDewPoint(float t, float rh) {
    // Magnus formula approximation
    float a = 17.27f, b = 237.7f;
    float alpha = ((a * t) / (b + t)) + log(rh / 100.0f);
    return (b * alpha) / (a - alpha);
}

void readSensors() {
    tempC    = dht.readTemperature();
    tempF    = dht.readTemperature(true);
    humidity = dht.readHumidity();
    ldrRaw   = analogRead(LDR_PIN);

    if (!isnan(tempC) && !isnan(humidity)) {
        heatIndex = dht.computeHeatIndex(tempC, humidity, false);
        dewPoint  = calcDewPoint(tempC, humidity);
    }
}

// ── OLED display ──────────────────────────────────────────────────────────────
void drawClockPage() {
    struct tm t;
    if (!getLocalTime(&t)) return;
    char timeBuf[10], dateBuf[20];
    strftime(timeBuf, sizeof(timeBuf), "%H:%M", &t);
    strftime(dateBuf, sizeof(dateBuf), "%a %d %b", &t);

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(3);
    display.setCursor(8, 8);
    display.print(timeBuf);
    display.setTextSize(1);
    display.setCursor(20, 50);
    display.print(dateBuf);
    display.display();
}

void drawClimatePage() {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

    display.setCursor(0, 0);  display.println("-- Climate --");
    display.setCursor(0, 12); display.printf("Temp:     %.1f C", tempC);
    display.setCursor(0, 22); display.printf("Humidity: %.1f %%", humidity);
    display.setCursor(0, 32); display.printf("Heat Idx: %.1f C", heatIndex);
    display.setCursor(0, 42); display.printf("Dew Pt:   %.1f C", dewPoint);
    display.setCursor(0, 55); display.print("zyron.co.za");
    display.display();
}

void drawLightPage() {
    int pct = map(ldrRaw, 0, 4095, 0, 100);
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0); display.println("-- Light Level --");
    display.setTextSize(3);
    display.setCursor(20, 20);
    display.printf("%3d%%", pct);
    display.setTextSize(1);
    display.setCursor(0, 55);
    display.printf("raw: %d", ldrRaw);
    display.display();
}

// ── MQTT ──────────────────────────────────────────────────────────────────────
void connectMQTT() {
    if (mqtt.connect(DEVICE_ID)) {
        mqtt.publish((String("zyron/") + DEVICE_ID + "/status").c_str(), "online", true);
    }
}

void publishMQTT() {
    if (!mqtt.connected()) connectMQTT();
    if (!mqtt.connected()) return;

    String base = String("zyron/") + DEVICE_ID + "/";
    if (!isnan(tempC))    mqtt.publish((base + "temperature").c_str(), String(tempC, 1).c_str());
    if (!isnan(humidity)) mqtt.publish((base + "humidity").c_str(),    String(humidity, 1).c_str());
    if (!isnan(heatIndex)) mqtt.publish((base + "heat_index").c_str(), String(heatIndex, 1).c_str());
    if (!isnan(dewPoint))  mqtt.publish((base + "dew_point").c_str(),  String(dewPoint, 1).c_str());
    mqtt.publish((base + "light_pct").c_str(), String(map(ldrRaw, 0, 4095, 0, 100)).c_str());
}

// ── Web API ───────────────────────────────────────────────────────────────────
void handleApiData() {
    struct tm t;
    char timeBuf[25] = "unknown";
    if (getLocalTime(&t)) strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%dT%H:%M:%S", &t);

    StaticJsonDocument<384> doc;
    doc["device_id"]      = DEVICE_ID;
    doc["timestamp"]      = timeBuf;
    doc["temperature_c"]  = isnan(tempC)    ? nullptr : serialized(String(tempC, 2));
    doc["temperature_f"]  = isnan(tempF)    ? nullptr : serialized(String(tempF, 2));
    doc["humidity_pct"]   = isnan(humidity) ? nullptr : serialized(String(humidity, 2));
    doc["heat_index_c"]   = isnan(heatIndex)? nullptr : serialized(String(heatIndex, 2));
    doc["dew_point_c"]    = isnan(dewPoint) ? nullptr : serialized(String(dewPoint, 2));
    doc["light_raw"]      = ldrRaw;
    doc["light_pct"]      = map(ldrRaw, 0, 4095, 0, 100);
    doc["uptime_s"]       = millis() / 1000;

    String json;
    serializeJson(doc, json);
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", json);
}

void handleRoot() {
    server.send(200, "text/html",
        "<!DOCTYPE html><html><head>"
        "<meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'>"
        "<title>Zyron Weather Station</title>"
        "<style>body{font-family:sans-serif;background:#0f0f1a;color:#eee;padding:20px}"
        "h1{color:#e94560}.grid{display:flex;flex-wrap:wrap;gap:12px}"
        ".c{background:#1a1a2e;border-radius:12px;padding:20px;min-width:160px;text-align:center}"
        ".l{font-size:.75em;color:#888;text-transform:uppercase;letter-spacing:1px}"
        ".v{font-size:2.2em;font-weight:bold;color:#e94560;margin:6px 0}"
        ".u{font-size:.85em;color:#aaa}"
        "</style></head><body>"
        "<h1>Zyron Weather Station</h1>"
        "<div class='grid' id='g'><div class='c'><div class='l'>Loading...</div></div></div>"
        "<p style='margin-top:20px;font-size:.8em;color:#555'>"
        "<a href='/data' style='color:#e94560'>JSON API</a> &nbsp;|&nbsp; zyron.co.za</p>"
        "<script>"
        "async function r(){"
        "const d=await(await fetch('/data')).json();"
        "document.getElementById('g').innerHTML=["
        "['Temperature',d.temperature_c,'°C'],"
        "['Humidity',d.humidity_pct,'%'],"
        "['Heat Index',d.heat_index_c,'°C'],"
        "['Dew Point',d.dew_point_c,'°C'],"
        "['Light',d.light_pct,'%'],"
        "['Uptime',d.uptime_s,'s']"
        "].map(([l,v,u])=>`<div class='c'><div class='l'>${l}</div>"
        "<div class='v'>${v??'ERR'}</div><div class='u'>${u}</div></div>`).join('');"
        "}r();setInterval(r,5000);"
        "</script></body></html>");
}

void setup() {
    Serial.begin(115200);
    dht.begin();

    if (display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        display.setTextColor(SSD1306_WHITE);
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(0, 10);
        display.println("Zyron Weather");
        display.println("Station v1.0");
        display.display();
    }

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) { delay(500); }
    Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());

    configTime(UTC_OFFSET, 0, "pool.ntp.org");
    mqtt.setServer(MQTT_SERVER, MQTT_PORT);

    // OTA — set a password before deploying to prevent unauthorised updates
    ArduinoOTA.setHostname(DEVICE_ID);
    ArduinoOTA.setPassword("YOUR_OTA_PASSWORD"); // REQUIRED — replace before use
    ArduinoOTA.begin();

    server.on("/",     handleRoot);
    server.on("/data", handleApiData);
    server.begin();

    delay(2000);
    readSensors();
    Serial.printf("Weather station running. Dashboard: http://%s/\n",
                  WiFi.localIP().toString().c_str());
}

void loop() {
    ArduinoOTA.handle();
    server.handleClient();
    if (mqtt.connected()) mqtt.loop();

    readSensors();

    // Rotate OLED pages every 5 seconds
    if (millis() - lastPageSwitch >= 5000) {
        lastPageSwitch = millis();
        displayPage = (displayPage + 1) % 3;
        switch (displayPage) {
            case 0: drawClockPage();   break;
            case 1: drawClimatePage(); break;
            case 2: drawLightPage();   break;
        }
    }

    // Publish MQTT every 30 seconds
    if (millis() - lastPublish >= PUBLISH_INTERVAL) {
        lastPublish = millis();
        publishMQTT();
    }
}
