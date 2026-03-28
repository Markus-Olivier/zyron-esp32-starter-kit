# Lesson 10 — Buzzer & Push Button

> **Zyron ESP32 Starter Kit Course** | [zyron.co.za](https://zyron.co.za)

---

## Part A: The Buzzer

### What Is a Buzzer?

A **buzzer** converts electrical energy into sound. Your kit includes an **active buzzer** — one that has a built-in oscillator circuit, so it beeps at a fixed frequency when you apply power.

![Active buzzer](../../images/components/buzzer.svg)
*Active buzzer — positive leg is longer and marked + on most modules*

### Active vs Passive

| Type | Has oscillator? | Control | Sounds |
|---|---|---|---|
| **Active** (your kit) | ✓ Yes | `digitalWrite(HIGH/LOW)` | One fixed frequency beep |
| **Passive** | ✗ No | `ledcWriteTone(channel, freq)` | Any frequency you choose (music!) |

If you want to play melodies, you need a **passive buzzer** and use the ESP32's LEDC PWM peripheral. Example 07 uses `ledcWriteTone()` — if your buzzer produces no tone variation, swap for a passive one.

### Wiring

```
Buzzer (+) positive leg  →  GPIO 25
Buzzer (−) negative leg  →  GND
```

Polarity matters — the buzzer will not sound if connected backwards (but it won't be damaged).

### Active Buzzer Code

```cpp
#define BUZZER_PIN 25

void setup() {
    pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
    digitalWrite(BUZZER_PIN, HIGH);  // ON — beep
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);   // OFF — silence
    delay(200);
}
```

### Passive Buzzer — Playing Tones with LEDC

```cpp
#define BUZZER_PIN 25
#define CHAN       0    // LEDC channel (0–15)

void setup() {
    ledcSetup(CHAN, 2000, 8);     // channel, initial freq, 8-bit resolution
    ledcAttachPin(BUZZER_PIN, CHAN);
}

void playTone(int freqHz, int durationMs) {
    ledcWriteTone(CHAN, freqHz);  // Start tone
    delay(durationMs);
    ledcWriteTone(CHAN, 0);       // Stop (silence)
    delay(50);                   // Brief gap between notes
}

void loop() {
    playTone(523, 300);  // C5
    playTone(587, 300);  // D5
    playTone(659, 300);  // E5
    playTone(698, 600);  // F5
    delay(1000);
}
```

### Musical Note Frequencies

| Note | Hz | Note | Hz | Note | Hz |
|---|---|---|---|---|---|
| C4 | 262 | E4 | 330 | G4 | 392 |
| D4 | 294 | F4 | 349 | A4 | 440 |
| B4 | 494 | C5 | 523 | D5 | 587 |
| E5 | 659 | F5 | 698 | G5 | 784 |

### Buzzer Alarm Patterns

```cpp
// SOS: 3 short, 3 long, 3 short
void sos() {
    for (int i = 0; i < 3; i++) { tone(100); }  // short
    for (int i = 0; i < 3; i++) { tone(300); }  // long
    for (int i = 0; i < 3; i++) { tone(100); }  // short
}

// Double beep alert
void alert() {
    digitalWrite(BUZZER_PIN, HIGH); delay(100);
    digitalWrite(BUZZER_PIN, LOW);  delay(100);
    digitalWrite(BUZZER_PIN, HIGH); delay(100);
    digitalWrite(BUZZER_PIN, LOW);  delay(500);
}
```

---

## Part B: The Push Button

### What Is a Push Button?

A **momentary push button** (tactile switch) connects two points while pressed and disconnects when released. It's the simplest form of user input.

![Push button](../../images/components/push_button.svg)
*4-leg tactile push button — fits across the centre gap of a breadboard*

### The 4-Leg Layout

```
  Leg A ──[internal connection]── Leg A  (same side — connected)
       |
     [button]
       |
  Leg B ──[internal connection]── Leg B  (same side — connected)

  Pressing bridges A to B.
  Insert button across the centre gap of the breadboard.
```

### Wiring with Internal Pull-Up

The simplest wiring uses the ESP32's built-in pull-up resistor:

```
  Button leg A  →  GPIO 15
  Button leg B  →  GND

  No external resistor needed.
```

With `INPUT_PULLUP`:
- GPIO reads **HIGH** normally (pulled up to 3.3V internally)
- GPIO reads **LOW** when button is pressed (pulls it to GND)

```cpp
#define BTN_PIN 15

void setup() {
    pinMode(BTN_PIN, INPUT_PULLUP); // HIGH = not pressed, LOW = pressed
}

void loop() {
    if (digitalRead(BTN_PIN) == LOW) {
        Serial.println("Button pressed!");
        delay(200); // Simple debounce
    }
}
```

### ⚠️ Switch Bounce (Why Debouncing Matters)

When you press a physical button, the metal contacts don't make a clean connection — they bounce rapidly for a few milliseconds, creating multiple fast transitions:

```
  What you do:     press ─────────────────── release
  What GPIO sees:  ─┐┌┐┌┐┌┐└───────────────┐┌┐┌┐┌┐─
                    │││││││                  │││││││
                  bounces                  bounces
```

Without debouncing, one button press might register as 5–20 presses. Fix it with software debouncing:

```cpp
bool      lastState  = HIGH;
uint32_t  lastChange = 0;
const int DEBOUNCE   = 50; // 50 milliseconds

void loop() {
    bool reading = digitalRead(BTN_PIN);

    if (reading != lastState) {
        lastChange = millis(); // Note when it changed
    }

    if (millis() - lastChange > DEBOUNCE) {
        // Signal has been stable for 50ms — it's a real press
        if (reading == LOW && lastState == HIGH) {
            Serial.println("Confirmed press");
        }
    }

    lastState = reading;
}
```

### Toggle Example

```cpp
bool ledOn = false;

// In loop, after debounce confirms press:
ledOn = !ledOn;
digitalWrite(LED_PIN, ledOn ? HIGH : LOW);
```

---

## Combining Buzzer + Button

A button-triggered buzzer makes a great doorbell or alert:

```cpp
#define BTN_PIN    15
#define BUZZER_PIN 25

void setup() {
    pinMode(BTN_PIN,    INPUT_PULLUP);
    pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
    if (digitalRead(BTN_PIN) == LOW) {
        // Play "ding-dong"
        digitalWrite(BUZZER_PIN, HIGH); delay(150);
        digitalWrite(BUZZER_PIN, LOW);  delay(100);
        digitalWrite(BUZZER_PIN, HIGH); delay(300);
        digitalWrite(BUZZER_PIN, LOW);
        delay(1000); // Prevent repeat triggering
    }
}
```

---

## Real-World Uses

**Buzzer:**
- Motion alarm beep (Example 05)
- Low-battery warning
- Button confirmation sound
- Melody / notification tones
- Error alerts

**Button:**
- Trigger an action (flash photo, send MQTT message)
- Toggle device state (relay on/off)
- Navigate menus on OLED display
- Reset/calibration trigger

---

## Course Complete!

You've now learned all the components in the Zyron ESP32 Starter Kit. Time to build something real — check out the 20 project examples for inspiration:

➡️ [Back to Course Index](README.md)
➡️ [Back to Main Project README](../../README.md)

---

*Built by [Zyron Software](https://zyron.co.za) — South African custom software engineering for robotics, IoT, and smart systems.*
