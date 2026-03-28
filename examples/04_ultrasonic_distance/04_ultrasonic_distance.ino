/*
 * 04 — HC-SR04 Ultrasonic Distance Sensor
 * Zyron ESP32 Starter Kit | https://zyron.co.za
 *
 * Measure the distance to the nearest object using the HC-SR04
 * ultrasonic sensor and print it to the Serial Monitor.
 *
 * Wiring:
 *   HC-SR04 VCC  → 5V (VIN pin on DevKit)
 *   HC-SR04 GND  → GND
 *   HC-SR04 TRIG → GPIO 5
 *   HC-SR04 ECHO → GPIO 18  ← IMPORTANT: use a voltage divider!
 *                             1kΩ between ECHO and GPIO 18,
 *                             2kΩ between GPIO 18 and GND.
 *                             The ECHO signal is 5V; ESP32 GPIO is 3.3V max.
 *
 * Skills: trigger/echo timing, pulseIn, unit conversion
 */

#define TRIG_PIN 5
#define ECHO_PIN 18

void setup() {
    Serial.begin(115200);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    digitalWrite(TRIG_PIN, LOW);
    Serial.println("Zyron ESP32 Starter Kit — 04 Ultrasonic Distance");
    Serial.println("-------------------------------------------------");
}

float measureDistanceCm() {
    // Ensure trigger is LOW before pulse
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    // Send 10 µs HIGH pulse
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    // Measure echo duration (timeout = 30 ms ≈ 5 m)
    long duration = pulseIn(ECHO_PIN, HIGH, 30000UL);
    if (duration == 0) return -1.0f; // out of range

    // Speed of sound: 343 m/s = 0.034 cm/µs
    return duration * 0.034f / 2.0f;
}

void loop() {
    float distCm   = measureDistanceCm();
    float distInch = distCm / 2.54f;

    if (distCm < 0) {
        Serial.println("Distance: OUT OF RANGE (> 400 cm)");
    } else {
        Serial.printf("Distance: %.1f cm  /  %.1f inches\n", distCm, distInch);
    }

    delay(500);
}
