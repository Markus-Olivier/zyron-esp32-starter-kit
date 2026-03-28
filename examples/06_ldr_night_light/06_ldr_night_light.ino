/*
 * 06 — LDR Auto Night Light
 * Zyron ESP32 Starter Kit | https://zyron.co.za
 *
 * Read ambient light level with an LDR (photoresistor) and
 * automatically switch a relay (or LED) ON when it gets dark.
 *
 * Wiring:
 *   LDR Circuit: 3.3V → LDR → GPIO 34 → 10kΩ → GND
 *   (GPIO 34 is ADC1_CH6, input-only pin — ideal for analog reads)
 *   Relay IN    → GPIO 26
 *   Relay VCC   → 5V (VIN)
 *   Relay GND   → GND
 *
 * Skills: analogRead, ADC, threshold logic, relay control
 */

#define LDR_PIN   34   // Analog input (ADC1 — works during WiFi too)
#define RELAY_PIN 26   // Relay control (LOW = ON for most relay modules)

// Threshold: below this raw ADC value (0–4095) is considered "dark"
// Adjust based on your LDR and ambient lighting conditions.
const int DARK_THRESHOLD = 1000;

// Hysteresis to prevent relay chattering at the threshold boundary
const int HYSTERESIS = 50;

bool relayOn = false;

void setup() {
    Serial.begin(115200);
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, HIGH); // Most relay modules: HIGH = OFF
    Serial.println("Zyron ESP32 Starter Kit — 06 LDR Night Light");
    Serial.println("----------------------------------------------");
}

void loop() {
    int raw      = analogRead(LDR_PIN);
    float volts  = raw * (3.3f / 4095.0f);
    int pct      = map(raw, 0, 4095, 0, 100);

    Serial.printf("Light: raw=%4d  voltage=%.2fV  level=%3d%%  Relay=%s\n",
                  raw, volts, pct, relayOn ? "ON" : "OFF");

    if (!relayOn && raw < DARK_THRESHOLD - HYSTERESIS) {
        relayOn = true;
        digitalWrite(RELAY_PIN, LOW);  // Activate relay
        Serial.println(">>> Dark detected — relay ON");
    } else if (relayOn && raw > DARK_THRESHOLD + HYSTERESIS) {
        relayOn = false;
        digitalWrite(RELAY_PIN, HIGH); // Deactivate relay
        Serial.println(">>> Light detected — relay OFF");
    }

    delay(1000);
}
