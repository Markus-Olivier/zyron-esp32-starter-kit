/*
 * 18 — SPIFFS Data Logger
 * Zyron ESP32 Starter Kit | https://zyron.co.za
 *
 * Log sensor readings to the ESP32's internal flash filesystem (SPIFFS/
 * LittleFS). Data survives power cycles. Download the log via a built-in
 * web server — no SD card needed.
 *
 * Features:
 *   - CSV log file stored on flash (/sensor_log.csv)
 *   - Configurable log interval
 *   - Web endpoint to download the CSV
 *   - Web endpoint to clear the log
 *   - Auto-truncates file when it exceeds MAX_LOG_SIZE_KB
 *
 * Wiring:
 *   DHT22 DATA → GPIO 4 (+ 10kΩ pull-up)
 *
 * Required Libraries:
 *   - SPIFFS (built-in)
 *   - DHT sensor library (Adafruit)
 *
 * Skills: SPIFFS, file I/O, CSV logging, web file download
 */

#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include <DHT.h>
#include "time.h"

// ── Configuration ─────────────────────────────────────────────────────────────
const char* WIFI_SSID        = "YOUR_WIFI_SSID";
const char* WIFI_PASS        = "YOUR_WIFI_PASSWORD";
const char* NTP_SERVER       = "pool.ntp.org";
const long  UTC_OFFSET       = 7200;  // UTC+2 (South Africa)
const uint32_t LOG_INTERVAL  = 30000; // Log every 30 seconds
const int MAX_LOG_SIZE_KB    = 512;   // Truncate when file exceeds this
const char* LOG_FILE         = "/sensor_log.csv";
// ──────────────────────────────────────────────────────────────────────────────

#define DHT_PIN   4
#define DHT_TYPE  DHT22

DHT       dht(DHT_PIN, DHT_TYPE);
WebServer server(80);
uint32_t  lastLog = 0;

String getTimestamp() {
    struct tm t;
    if (!getLocalTime(&t)) return "0000-00-00 00:00:00";
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &t);
    return String(buf);
}

void appendLog(const String& line) {
    // Truncate if too large
    File f = SPIFFS.open(LOG_FILE, "r");
    if (f && f.size() > MAX_LOG_SIZE_KB * 1024) {
        f.close();
        SPIFFS.remove(LOG_FILE);
        Serial.println("[LOG] File truncated (size limit reached).");
    } else if (f) {
        f.close();
    }

    File log = SPIFFS.open(LOG_FILE, "a");
    if (!log) { Serial.println("[ERROR] Cannot open log file."); return; }
    log.println(line);
    log.close();
}

void handleDownload() {
    File f = SPIFFS.open(LOG_FILE, "r");
    if (!f) {
        server.send(404, "text/plain", "Log file not found.");
        return;
    }
    server.sendHeader("Content-Disposition", "attachment; filename=sensor_log.csv");
    server.streamFile(f, "text/csv");
    f.close();
}

void handleClear() {
    SPIFFS.remove(LOG_FILE);
    // Re-create with CSV header
    File f = SPIFFS.open(LOG_FILE, "w");
    if (f) { f.println("timestamp,temperature_c,humidity_pct"); f.close(); }
    server.send(200, "text/plain", "Log cleared.");
    Serial.println("[LOG] Cleared.");
}

void handleStatus() {
    File f = SPIFFS.open(LOG_FILE, "r");
    size_t size = f ? f.size() : 0;
    if (f) f.close();

    FSInfo info;
    SPIFFS.info(info);

    String body = "Log file: " + String(LOG_FILE) + "\n";
    body += "Size: " + String(size / 1024.0f, 1) + " KB\n";
    body += "Flash used: " + String(info.usedBytes / 1024.0f, 1) + " / "
          + String(info.totalBytes / 1024.0f, 1) + " KB\n";
    body += "Endpoints:\n  /log  → download CSV\n  /clear → clear log\n";
    server.send(200, "text/plain", body);
}

void logSensors() {
    float temp = dht.readTemperature();
    float hum  = dht.readHumidity();
    if (isnan(temp) || isnan(hum)) {
        Serial.println("[LOG] DHT22 error — skipping.");
        return;
    }

    String line = getTimestamp() + "," + String(temp, 2) + "," + String(hum, 2);
    appendLog(line);
    Serial.printf("[LOG] %s\n", line.c_str());
}

void setup() {
    Serial.begin(115200);
    dht.begin();

    if (!SPIFFS.begin(true)) {
        Serial.println("[ERROR] SPIFFS mount failed.");
        while (true);
    }

    // Create header if new file
    if (!SPIFFS.exists(LOG_FILE)) {
        File f = SPIFFS.open(LOG_FILE, "w");
        if (f) { f.println("timestamp,temperature_c,humidity_pct"); f.close(); }
    }

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
    Serial.printf("\nIP: %s\n", WiFi.localIP().toString().c_str());

    configTime(UTC_OFFSET, 0, NTP_SERVER);

    server.on("/",      handleStatus);
    server.on("/log",   handleDownload);
    server.on("/clear", handleClear);
    server.begin();

    Serial.printf("Download log: http://%s/log\n", WiFi.localIP().toString().c_str());
}

void loop() {
    server.handleClient();

    if (millis() - lastLog >= LOG_INTERVAL) {
        lastLog = millis();
        logSensors();
    }
}
