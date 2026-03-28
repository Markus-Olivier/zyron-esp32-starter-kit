/*
 * 11 — WiFi Sensor Dashboard
 * Zyron ESP32 Starter Kit | https://zyron.co.za
 *
 * Serve a live, auto-refreshing web dashboard showing all sensor
 * readings over your local WiFi network. No cloud required.
 *
 * Wiring:
 *   DHT22 DATA   → GPIO 4  (+ 10kΩ pull-up)
 *   HC-SR04 TRIG → GPIO 5
 *   HC-SR04 ECHO → GPIO 18 (voltage divider!)
 *   PIR OUT      → GPIO 13
 *
 * Required Libraries:
 *   - DHT sensor library (Adafruit)
 *   - Adafruit Unified Sensor
 *
 * Skills: WiFi, WebServer, JSON API, live data dashboard, CORS
 */

#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
#include <ArduinoJson.h>

// ── Configuration ─────────────────────────────────────────────────────────────
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";
// ──────────────────────────────────────────────────────────────────────────────

#define DHT_PIN   4
#define DHT_TYPE  DHT22
#define TRIG_PIN  5
#define ECHO_PIN  18
#define PIR_PIN   13

DHT      dht(DHT_PIN, DHT_TYPE);
WebServer server(80);

float measureDistanceCm() {
    digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    long dur = pulseIn(ECHO_PIN, HIGH, 30000UL);
    return (dur == 0) ? -1.0f : dur * 0.034f / 2.0f;
}

// ── JSON API endpoint ─────────────────────────────────────────────────────────
void handleApi() {
    float temp  = dht.readTemperature();
    float hum   = dht.readHumidity();
    float dist  = measureDistanceCm();
    bool  motion = digitalRead(PIR_PIN) == HIGH;

    StaticJsonDocument<256> doc;
    doc["temperature_c"] = isnan(temp) ? nullptr : serialized(String(temp, 1));
    doc["humidity_pct"]  = isnan(hum)  ? nullptr : serialized(String(hum, 1));
    doc["distance_cm"]   = dist < 0 ? -1 : dist;
    doc["motion"]        = motion;
    doc["uptime_s"]      = millis() / 1000;

    String json;
    serializeJson(doc, json);
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", json);
}

// ── HTML dashboard ────────────────────────────────────────────────────────────
void handleRoot() {
    String html = R"rawhtml(<!DOCTYPE html>
<html lang="en"><head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Zyron Sensor Dashboard</title>
<style>
  body{margin:0;font-family:sans-serif;background:#0f0f1a;color:#eee}
  header{background:#e94560;padding:16px 24px}
  header h1{margin:0;font-size:1.4em}
  header p{margin:4px 0 0;font-size:.85em;opacity:.8}
  .grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(180px,1fr));gap:16px;padding:24px}
  .card{background:#1a1a2e;border-radius:12px;padding:20px;text-align:center}
  .card .label{font-size:.75em;color:#888;text-transform:uppercase;letter-spacing:1px}
  .card .value{font-size:2.2em;font-weight:bold;margin:8px 0;color:#e94560}
  .card .unit{font-size:.9em;color:#aaa}
  .motion-true .value{color:#27ae60} .motion-false .value{color:#555}
  footer{text-align:center;padding:16px;font-size:.75em;color:#555}
</style>
</head><body>
<header><h1>Zyron Sensor Dashboard</h1><p>ESP32 Live Readings — zyron.co.za</p></header>
<div class="grid">
  <div class="card"><div class="label">Temperature</div><div class="value" id="temp">--</div><div class="unit">°C</div></div>
  <div class="card"><div class="label">Humidity</div><div class="value" id="hum">--</div><div class="unit">%</div></div>
  <div class="card"><div class="label">Distance</div><div class="value" id="dist">--</div><div class="unit">cm</div></div>
  <div class="card" id="motion-card"><div class="label">Motion</div><div class="value" id="motion">--</div><div class="unit"></div></div>
  <div class="card"><div class="label">Uptime</div><div class="value" id="uptime">--</div><div class="unit">seconds</div></div>
</div>
<footer>Built with Zyron ESP32 Starter Kit | <a href="/api" style="color:#e94560">JSON API</a></footer>
<script>
async function refresh() {
  try {
    const r = await fetch('/api');
    const d = await r.json();
    document.getElementById('temp').textContent   = d.temperature_c ?? 'ERR';
    document.getElementById('hum').textContent    = d.humidity_pct ?? 'ERR';
    document.getElementById('dist').textContent   = d.distance_cm < 0 ? '>4m' : d.distance_cm.toFixed(1);
    document.getElementById('motion').textContent = d.motion ? 'YES' : 'NO';
    document.getElementById('uptime').textContent = d.uptime_s;
    const mc = document.getElementById('motion-card');
    mc.className = 'card motion-' + d.motion;
  } catch(e) { console.error(e); }
}
refresh();
setInterval(refresh, 1000);
</script>
</body></html>)rawhtml";
    server.send(200, "text/html", html);
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
    Serial.printf("\nDashboard: http://%s\n", WiFi.localIP().toString().c_str());
    Serial.printf("JSON API:  http://%s/api\n", WiFi.localIP().toString().c_str());

    server.on("/",    handleRoot);
    server.on("/api", handleApi);
    server.begin();
}

void loop() {
    server.handleClient();
}
