/*
 * 09 — Relay Web Control
 * Zyron ESP32 Starter Kit | https://zyron.co.za
 *
 * Control a relay (and anything connected to it) from any browser
 * on your WiFi network. A minimal HTTP server serves a control page.
 *
 * Wiring:
 *   Relay IN  → GPIO 26
 *   Relay VCC → 5V (VIN)
 *   Relay GND → GND
 *
 *   CAUTION: When switching mains AC (230V) through the relay,
 *   ensure the relay is rated for the load and follow electrical
 *   safety guidelines. Never work on live circuits.
 *
 * Required Libraries:
 *   - WiFi (built-in with ESP32 Arduino core)
 *
 * Skills: WiFi AP/STA, HTTP server, HTML generation, relay control
 */

#include <WiFi.h>
#include <WebServer.h>

// ── Configuration ─────────────────────────────────────────────────────────────
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";
// ──────────────────────────────────────────────────────────────────────────────

#define RELAY_PIN 26

WebServer server(80);
bool relayState = false;

String buildPage() {
    String color  = relayState ? "#27ae60" : "#c0392b";
    String status = relayState ? "ON" : "OFF";
    String btnLabel = relayState ? "Turn OFF" : "Turn ON";

    return String("<!DOCTYPE html><html><head>"
        "<meta charset='utf-8'>"
        "<meta name='viewport' content='width=device-width,initial-scale=1'>"
        "<title>Zyron Relay Control</title>"
        "<style>body{font-family:sans-serif;text-align:center;padding:40px;background:#1a1a2e;color:#eee}"
        "h1{color:#e94560}h2{color:") + color + "}"
        ".btn{display:inline-block;padding:16px 40px;font-size:1.2em;border:none;"
        "border-radius:8px;cursor:pointer;background:#e94560;color:#fff;text-decoration:none}"
        ".btn:hover{background:#c0392b}"
        "</style></head><body>"
        "<h1>Zyron Relay Control</h1>"
        "<h2>Relay is <strong>" + status + "</strong></h2>"
        "<a class='btn' href='/toggle'>" + btnLabel + "</a>"
        "<p style='margin-top:40px;font-size:0.8em;color:#888'>zyron.co.za</p>"
        "</body></html>";
}

void handleRoot() {
    server.send(200, "text/html", buildPage());
}

void handleToggle() {
    relayState = !relayState;
    digitalWrite(RELAY_PIN, relayState ? LOW : HIGH); // LOW = ON for most relay modules
    Serial.printf("Relay toggled: %s\n", relayState ? "ON" : "OFF");
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "");
}

void setup() {
    Serial.begin(115200);
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, HIGH); // Start with relay OFF

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.printf("\nConnected! IP: http://%s\n", WiFi.localIP().toString().c_str());
    Serial.println("Open the above URL in your browser.");

    server.on("/", handleRoot);
    server.on("/toggle", handleToggle);
    server.begin();
    Serial.println("HTTP server started.");
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi lost — reconnecting...");
        WiFi.disconnect();
        WiFi.begin(WIFI_SSID, WIFI_PASS);
        uint32_t start = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
            delay(500);
            Serial.print(".");
        }
        if (WiFi.status() == WL_CONNECTED) {
            Serial.printf("\nReconnected. IP: http://%s\n", WiFi.localIP().toString().c_str());
        }
    }
    server.handleClient();
}
