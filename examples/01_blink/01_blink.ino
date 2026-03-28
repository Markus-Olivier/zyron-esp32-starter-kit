/*
 * 01 — Blink
 * Zyron ESP32 Starter Kit | https://zyron.co.za
 *
 * The "Hello World" of hardware: blink the onboard LED.
 * No extra components needed.
 *
 * Wiring:
 *   Built-in LED on GPIO 2 (most ESP32 DevKit v1 boards)
 *   Or connect an external LED + 220Ω resistor to LED_PIN.
 *
 * Skills: pinMode, digitalWrite, delay
 */

#define LED_PIN 2   // Built-in LED on most ESP32 DevKit boards

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    Serial.println("Zyron ESP32 Starter Kit — 01 Blink");
}

void loop() {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED ON");
    delay(1000);

    digitalWrite(LED_PIN, LOW);
    Serial.println("LED OFF");
    delay(1000);
}
