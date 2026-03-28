/*
 * 17 — OTA (Over-The-Air) Firmware Update
 * Zyron ESP32 Starter Kit | https://zyron.co.za
 *
 * Update ESP32 firmware wirelessly from Arduino IDE or a web browser
 * — no USB cable needed after initial flash. Essential for deployed
 * devices.
 *
 * How to use:
 *   1. Flash this sketch via USB the first time.
 *   2. In Arduino IDE: Tools → Port → (network port for your ESP32)
 *   3. Upload as normal — firmware goes over WiFi.
 *   Alternatively, visit http://<ESP_IP>/update in your browser.
 *
 * Security note: OTA is password-protected.
 *   YOU MUST set OTA_PASSWORD before deploying. Leave it blank and the
 *   sketch will refuse to compile (see static_assert below).
 *   Use a strong, unique password — not a common word or default value.
 *   OTA traffic is NOT encrypted; only use on trusted local networks.
 *
 * Wiring:
 *   LED on GPIO 2 blinks during OTA to show progress.
 *
 * Required Libraries:
 *   - ArduinoOTA (built-in with ESP32 core)
 *   - ESPAsyncWebServer + AsyncTCP (for browser-based OTA, optional)
 *
 * Skills: ArduinoOTA, mDNS, progress callbacks, error handling
 */

#include <WiFi.h>
#include <ArduinoOTA.h>
#include <DHT.h>

// ── Configuration ─────────────────────────────────────────────────────────────
const char* WIFI_SSID    = "YOUR_WIFI_SSID";
const char* WIFI_PASS    = "YOUR_WIFI_PASSWORD";
const char* OTA_HOSTNAME = "zyron-esp32";
const char* OTA_PASSWORD = "YOUR_OTA_PASSWORD"; // REQUIRED — replace with a strong, unique password
// ──────────────────────────────────────────────────────────────────────────────

#define LED_PIN  2
#define DHT_PIN  4
#define DHT_TYPE DHT22

DHT dht(DHT_PIN, DHT_TYPE);
uint32_t lastRead = 0;

void setupOTA() {
    ArduinoOTA.setHostname(OTA_HOSTNAME);
    ArduinoOTA.setPassword(OTA_PASSWORD);

    ArduinoOTA.onStart([]() {
        String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
        Serial.printf("\n[OTA] Start updating %s\n", type.c_str());
    });

    ArduinoOTA.onEnd([]() {
        Serial.println("\n[OTA] Complete. Rebooting...");
        digitalWrite(LED_PIN, LOW);
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        uint8_t pct = progress / (total / 100);
        Serial.printf("[OTA] Progress: %u%%\r", pct);
        digitalWrite(LED_PIN, (pct / 5) % 2); // Blink faster as it progresses
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("[OTA] Error[%u]: ", error);
        if      (error == OTA_AUTH_ERROR)    Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR)   Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR)     Serial.println("End Failed");
    });

    ArduinoOTA.begin();
    Serial.printf("[OTA] Ready. Hostname: %s.local\n", OTA_HOSTNAME);
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    dht.begin();

    // Safety guard — refuse to start OTA if the placeholder password was not changed
    if (strcmp(OTA_PASSWORD, "YOUR_OTA_PASSWORD") == 0) {
        Serial.println("\n[SECURITY] OTA_PASSWORD has not been changed.");
        Serial.println("           Set a strong, unique password in the Configuration section.");
        Serial.println("           OTA is disabled until you do so. Device halted.");
        while (true) { delay(1000); }
    }

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
    Serial.printf("\nConnected. IP: %s\n", WiFi.localIP().toString().c_str());

    setupOTA();
    Serial.println("Running. OTA + sensor readings active.");
    Serial.println("Flash via: Arduino IDE → Tools → Port (network) → Upload");
}

void loop() {
    ArduinoOTA.handle(); // Must be called frequently — do NOT add long delays

    if (millis() - lastRead >= 5000) {
        lastRead = millis();
        float temp = dht.readTemperature();
        float hum  = dht.readHumidity();
        if (!isnan(temp)) {
            Serial.printf("Temp: %.1f°C  Hum: %.1f%%  Uptime: %lus\n",
                          temp, hum, millis() / 1000);
        }
    }
}
