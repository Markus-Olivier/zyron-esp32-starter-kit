/*
 * 02 — Button + LED
 * Zyron ESP32 Starter Kit | https://zyron.co.za
 *
 * Read a push button and toggle an LED. Demonstrates digital
 * input with internal pull-up resistor and debouncing.
 *
 * Wiring:
 *   Button  → GPIO 15 (one leg to GND, other to GPIO 15)
 *   LED     → GPIO 2 via 220Ω resistor to GND
 *
 * Skills: digital input, pull-up resistors, software debounce, toggle state
 */

#define BTN_PIN  15
#define LED_PIN  2

bool      ledState     = false;
bool      lastBtnState = HIGH;
uint32_t  lastDebounce = 0;
const uint32_t DEBOUNCE_MS = 50;

void setup() {
    Serial.begin(115200);
    pinMode(BTN_PIN, INPUT_PULLUP);  // HIGH when not pressed, LOW when pressed
    pinMode(LED_PIN, OUTPUT);
    Serial.println("Zyron ESP32 Starter Kit — 02 Button + LED");
    Serial.println("Press the button to toggle the LED.");
}

void loop() {
    bool reading = digitalRead(BTN_PIN);

    if (reading != lastBtnState) {
        lastDebounce = millis();
    }

    if ((millis() - lastDebounce) > DEBOUNCE_MS) {
        // Button just pressed (HIGH→LOW transition with pull-up)
        if (reading == LOW && lastBtnState == HIGH) {
            ledState = !ledState;
            digitalWrite(LED_PIN, ledState ? HIGH : LOW);
            Serial.printf("Button pressed — LED is now %s\n", ledState ? "ON" : "OFF");
        }
    }

    lastBtnState = reading;
}
