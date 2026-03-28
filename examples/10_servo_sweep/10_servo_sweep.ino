/*
 * 10 — Servo Motor Control
 * Zyron ESP32 Starter Kit | https://zyron.co.za
 *
 * Control an SG90 (or similar) servo motor. Demonstrates a smooth
 * sweep from 0° to 180° and back, plus potentiometer control.
 *
 * Wiring:
 *   Servo Brown/Black → GND
 *   Servo Red         → 5V (VIN) — servos need 5V, not 3.3V
 *   Servo Orange/Yellow → GPIO 13
 *
 *   Potentiometer (optional manual control):
 *   Pot left  → GND
 *   Pot right → 3.3V
 *   Pot wiper → GPIO 34
 *
 * Required Libraries:
 *   - ESP32Servo (by Kevin Harrington)
 *
 * Skills: PWM servo control, LEDC, analog mapping, smooth motion
 */

#include <ESP32Servo.h>

#define SERVO_PIN 13
#define POT_PIN   34   // Potentiometer (optional)

Servo myServo;

// Smooth move: step from current angle to target
void moveTo(int targetAngle, int stepDelayMs = 15) {
    int current = myServo.read();
    int step    = (targetAngle > current) ? 1 : -1;

    while (current != targetAngle) {
        current += step;
        myServo.write(current);
        delay(stepDelayMs);
    }
}

void setup() {
    Serial.begin(115200);
    ESP32PWM::allocateTimer(0);
    myServo.setPeriodHertz(50);         // Standard 50 Hz servo
    myServo.attach(SERVO_PIN, 500, 2400); // Min/max pulse width (µs)
    Serial.println("Zyron ESP32 Starter Kit — 10 Servo Control");
    Serial.println("Comment/uncomment modes in loop() to switch behaviour.");
}

void sweepMode() {
    Serial.println("Sweeping 0° → 180°");
    moveTo(180);
    delay(500);
    Serial.println("Sweeping 180° → 0°");
    moveTo(0);
    delay(500);
}

void potMode() {
    int raw   = analogRead(POT_PIN);
    int angle = map(raw, 0, 4095, 0, 180);
    myServo.write(angle);
    Serial.printf("Pot: %4d → %3d°\n", raw, angle);
    delay(20);
}

void loop() {
    sweepMode();
    // potMode();   // Uncomment to use potentiometer control instead
}
