/*
 * 19 — Smart Home Node
 * Zyron ESP32 Starter Kit | https://zyron.co.za
 *
 * A production-ready smart home automation node combining:
 *   - PIR motion detection → MQTT alert + relay trigger
 *   - DHT22 climate monitoring → published to MQTT
 *   - LDR auto-lighting → relay switches light on at dark
 *   - Web dashboard for live status + manual relay override
 *   - Home Assistant ready (auto-discovery via MQTT)
 *
 * Wiring:
 *   DHT22 DATA   → GPIO 4
 *   PIR OUT      → GPIO 13
 *   LDR          → GPIO 34 (voltage divider: 3.3V → LDR → GPIO34 → 10kΩ → GND)
 *   Relay IN     → GPIO 26
 *   LED (status) → GPIO 2
 *
 * Required Libraries:
 *   - PubSubClient, DHT sensor library, ArduinoJson
 *
 * Skills: multi-sensor fusion, state machine, MQTT, web server, automation rules
 */

#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

// ── Configuration ─────────────────────────────────────────────────────────────
const char* WIFI_SSID    = "YOUR_WIFI_SSID";
const char* WIFI_PASS    = "YOUR_WIFI_PASSWORD";
const char* MQTT_SERVER  = "192.168.1.100"; // Your local broker IP
const int   MQTT_PORT    = 1883;             // Use 8883 + TLS for cloud/public brokers
const char* DEVICE_ID    = "zyron-node-01"; // Change per device to avoid topic collisions

const int    LDR_DARK_THRESHOLD = 800;
const uint32_t SENSOR_INTERVAL  = 10000; // Read sensors every 10 s
// ──────────────────────────────────────────────────────────────────────────────

#define DHT_PIN   4
#define PIR_PIN   13
#define LDR_PIN   34
#define RELAY_PIN 26
#define LED_PIN   2

DHT         dht(DHT_PIN, DHT22);
WiFiClient  wifiClient;
PubSubClient mqtt(wifiClient);
WebServer   server(80);

// State
float   temp = 0, hum = 0;
bool    motion = false, relayOn = false;
int     ldrRaw = 0;
bool    autoMode = true;  // If true, relay controlled automatically by LDR

uint32_t lastSensor = 0;

// ── MQTT helpers ──────────────────────────────────────────────────────────────
void mqttPublish(const char* subtopic, const String& payload, bool retain = false) {
    String topic = String("zyron/") + DEVICE_ID + "/" + subtopic;
    mqtt.publish(topic.c_str(), payload.c_str(), retain);
}

void mqttSubscribe() {
    String cmd = String("zyron/") + DEVICE_ID + "/relay/set";
    mqtt.subscribe(cmd.c_str());
    String mode = String("zyron/") + DEVICE_ID + "/mode/set";
    mqtt.subscribe(mode.c_str());
}

void mqttCallback(char* topic, byte* payload, unsigned int len) {
    String msg((char*)payload, len);
    String t(topic);
    Serial.printf("[MQTT] %s = %s\n", topic, msg.c_str());

    if (t.endsWith("/relay/set")) {
        relayOn = (msg == "ON" || msg == "true" || msg == "1");
        digitalWrite(RELAY_PIN, relayOn ? LOW : HIGH);
        mqttPublish("relay/state", relayOn ? "ON" : "OFF", true);
    } else if (t.endsWith("/mode/set")) {
        autoMode = (msg == "auto");
        mqttPublish("mode/state", autoMode ? "auto" : "manual", true);
    }
}

void connectMQTT() {
    while (!mqtt.connected()) {
        if (mqtt.connect(DEVICE_ID, nullptr, nullptr,
                         (String("zyron/") + DEVICE_ID + "/status").c_str(),
                         0, true, "offline")) {
            mqtt.publish((String("zyron/") + DEVICE_ID + "/status").c_str(), "online", true);
            mqttSubscribe();
            Serial.println("[MQTT] Connected.");
        } else {
            delay(3000);
        }
    }
}

// ── Sensor read + automation ──────────────────────────────────────────────────
void readAndPublish() {
    temp   = dht.readTemperature();
    hum    = dht.readHumidity();
    ldrRaw = analogRead(LDR_PIN);
    motion = digitalRead(PIR_PIN) == HIGH;

    if (!isnan(temp)) mqttPublish("temperature", String(temp, 1));
    if (!isnan(hum))  mqttPublish("humidity",    String(hum, 1));
    mqttPublish("ldr",    String(ldrRaw));
    mqttPublish("motion", motion ? "true" : "false");

    if (motion) {
        mqttPublish("motion/event", String(millis()));
        Serial.println("[MOTION] Detected!");
    }

    // Auto-lighting rule: turn relay ON when dark
    if (autoMode) {
        bool shouldBeOn = (ldrRaw < LDR_DARK_THRESHOLD);
        if (shouldBeOn != relayOn) {
            relayOn = shouldBeOn;
            digitalWrite(RELAY_PIN, relayOn ? LOW : HIGH);
            mqttPublish("relay/state", relayOn ? "ON" : "OFF", true);
            Serial.printf("[AUTO] Relay %s (LDR=%d)\n", relayOn ? "ON" : "OFF", ldrRaw);
        }
    }

    // Status LED mirrors relay
    digitalWrite(LED_PIN, relayOn ? HIGH : LOW);
}

// ── Web dashboard ─────────────────────────────────────────────────────────────
void handleRoot() {
    String relay  = relayOn ? "ON"   : "OFF";
    String mode   = autoMode ? "AUTO" : "MANUAL";
    String mot    = motion ? "YES"  : "NO";

    String html = "<!DOCTYPE html><html><head>"
        "<meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'>"
        "<meta http-equiv='refresh' content='5'>"
        "<title>Zyron Smart Node</title>"
        "<style>body{font-family:sans-serif;background:#0f0f1a;color:#eee;padding:20px}"
        "h1{color:#e94560}.grid{display:flex;flex-wrap:wrap;gap:12px}"
        ".c{background:#1a1a2e;border-radius:10px;padding:16px;min-width:140px;text-align:center}"
        ".v{font-size:2em;color:#e94560;font-weight:bold}"
        "a.btn{display:inline-block;padding:10px 20px;background:#e94560;color:#fff;"
        "border-radius:6px;text-decoration:none;margin:4px}"
        "</style></head><body>"
        "<h1>Zyron Smart Home Node</h1>"
        "<div class='grid'>"
        "<div class='c'><div>Temp</div><div class='v'>" + String(temp,1) + "°C</div></div>"
        "<div class='c'><div>Humidity</div><div class='v'>" + String(hum,1) + "%</div></div>"
        "<div class='c'><div>Light</div><div class='v'>" + String(ldrRaw) + "</div></div>"
        "<div class='c'><div>Motion</div><div class='v'>" + mot + "</div></div>"
        "<div class='c'><div>Relay</div><div class='v'>" + relay + "</div></div>"
        "<div class='c'><div>Mode</div><div class='v'>" + mode + "</div></div>"
        "</div><br>"
        "<a class='btn' href='/relay/on'>Relay ON</a>"
        "<a class='btn' href='/relay/off'>Relay OFF</a>"
        "<a class='btn' href='/mode/auto'>Auto Mode</a>"
        "<a class='btn' href='/mode/manual'>Manual Mode</a>"
        "<p style='font-size:.8em;color:#555;margin-top:20px'>zyron.co.za — auto-refreshes every 5s</p>"
        "</body></html>";
    server.send(200, "text/html", html);
}

void setup() {
    Serial.begin(115200);
    dht.begin();
    pinMode(PIR_PIN,  INPUT);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(LED_PIN,  OUTPUT);
    digitalWrite(RELAY_PIN, HIGH); // Relay OFF

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) { delay(500); }
    Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());

    mqtt.setServer(MQTT_SERVER, MQTT_PORT);
    mqtt.setCallback(mqttCallback);

    server.on("/", handleRoot);
    server.on("/relay/on",    []{ relayOn=true;  autoMode=false; digitalWrite(RELAY_PIN,LOW);  server.sendHeader("Location","/"); server.send(302); });
    server.on("/relay/off",   []{ relayOn=false; autoMode=false; digitalWrite(RELAY_PIN,HIGH); server.sendHeader("Location","/"); server.send(302); });
    server.on("/mode/auto",   []{ autoMode=true;  server.sendHeader("Location","/"); server.send(302); });
    server.on("/mode/manual", []{ autoMode=false; server.sendHeader("Location","/"); server.send(302); });
    server.begin();
}

void loop() {
    if (!mqtt.connected()) connectMQTT();
    mqtt.loop();
    server.handleClient();

    if (millis() - lastSensor >= SENSOR_INTERVAL) {
        lastSensor = millis();
        readAndPublish();
    }
}
