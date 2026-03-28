/*
 * 14 — BLE Presence Detector / Scanner
 * Zyron ESP32 Starter Kit | https://zyron.co.za
 *
 * Scan for nearby Bluetooth Low Energy (BLE) devices and report
 * their MAC addresses, names, and signal strength (RSSI).
 * Useful for presence detection — e.g. detect when your phone is home.
 *
 * Wiring:
 *   No extra hardware required — uses the ESP32's built-in BLE radio.
 *   Optional: LED on GPIO 2 to indicate when a tracked device is present.
 *
 * Skills: BLE scanning, RSSI, MAC filtering, presence detection
 *
 * NOTE: Add MAC addresses of devices you want to track in TRACKED_MACS.
 * Find your phone's BLE MAC: Settings → Bluetooth → device info.
 */

#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#define LED_PIN       2
#define SCAN_TIME_S   5   // How long each BLE scan runs

// Add MAC addresses to track (lowercase, colon-separated)
const char* TRACKED_MACS[] = {
    "aa:bb:cc:dd:ee:ff",  // Replace with your device MAC
};
const int TRACKED_COUNT = sizeof(TRACKED_MACS) / sizeof(TRACKED_MACS[0]);

BLEScan* pBLEScan;
bool     trackedPresent = false;

class ScanCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice device) override {
        String mac  = device.getAddress().toString().c_str();
        String name = device.haveName() ? device.getName().c_str() : "(unknown)";
        int    rssi = device.getRSSI();

        Serial.printf("  [BLE] %s  %-30s  RSSI: %d dBm\n",
                      mac.c_str(), name.c_str(), rssi);

        // Check if this is a tracked device
        for (int i = 0; i < TRACKED_COUNT; i++) {
            if (mac.equalsIgnoreCase(TRACKED_MACS[i])) {
                Serial.printf("  >>> TRACKED DEVICE FOUND: %s (RSSI %d)\n",
                              mac.c_str(), rssi);
                trackedPresent = true;
            }
        }
    }
};

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);

    BLEDevice::init("ZyronESP32-Scanner");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new ScanCallbacks());
    pBLEScan->setActiveScan(true);   // Active scan: request device names
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);

    Serial.println("Zyron ESP32 Starter Kit — 14 BLE Scanner");
    Serial.printf("Scanning every %d seconds. Tracking %d device(s).\n",
                  SCAN_TIME_S, TRACKED_COUNT);
}

void loop() {
    trackedPresent = false;
    Serial.printf("\n--- BLE Scan (%d s) ---\n", SCAN_TIME_S);

    BLEScanResults results = pBLEScan->start(SCAN_TIME_S, false);
    Serial.printf("--- Found %d device(s) ---\n", results.getCount());

    digitalWrite(LED_PIN, trackedPresent ? HIGH : LOW);

    if (trackedPresent) {
        Serial.println("STATUS: Tracked device IS present.");
    } else {
        Serial.println("STATUS: No tracked devices in range.");
    }

    pBLEScan->clearResults();
    delay(2000);
}
