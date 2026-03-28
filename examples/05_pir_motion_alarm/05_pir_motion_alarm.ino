/*
 * 05 — PIR Motion Alarm
 * Zyron ESP32 Starter Kit | https://zyron.co.za
 *
 * Detect motion with an HC-SR501 PIR sensor and trigger a buzzer
 * + LED alarm. Prints events to Serial with timestamps.
 *
 * Wiring:
 *   PIR VCC    → 5V (VIN)
 *   PIR GND    → GND
 *   PIR OUT    → GPIO 13
 *   Buzzer (+) → GPIO 25  (active buzzer, positive leg)
 *   Buzzer (-) → GND
 *   LED        → GPIO 2 via 220Ω resistor
 *
 * HC-SR501 tip: After power-on, allow 30–60 seconds for the sensor
 * to calibrate before it will detect motion reliably.
 *
 * Skills: interrupt-driven input, millis() timing, alarm state machine
 */

#define PIR_PIN    13
#define BUZZER_PIN 25
#define LED_PIN    2

#define ALARM_DURATION_MS 3000  // How long to sound the alarm after detection

volatile bool motionFlag = false;
uint32_t alarmStart      = 0;
bool     alarmActive     = false;

void IRAM_ATTR onMotion() {
    motionFlag = true;
}

void setup() {
    Serial.begin(115200);
    pinMode(PIR_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, LOW);

    attachInterrupt(digitalPinToInterrupt(PIR_PIN), onMotion, RISING);

    Serial.println("Zyron ESP32 Starter Kit — 05 PIR Motion Alarm");
    Serial.println("Waiting for PIR to calibrate (~30s)...");
    delay(30000);
    Serial.println("Ready. Monitoring for motion.");
}

void loop() {
    // Handle new motion event
    if (motionFlag) {
        motionFlag  = false;
        alarmActive = true;
        alarmStart  = millis();

        Serial.printf("[ALERT] Motion detected at %lu ms\n", alarmStart);
        digitalWrite(BUZZER_PIN, HIGH);
        digitalWrite(LED_PIN, HIGH);
    }

    // Turn off alarm after duration
    if (alarmActive && (millis() - alarmStart >= ALARM_DURATION_MS)) {
        alarmActive = false;
        digitalWrite(BUZZER_PIN, LOW);
        digitalWrite(LED_PIN, LOW);
        Serial.println("[INFO]  Alarm cleared.");
    }
}
