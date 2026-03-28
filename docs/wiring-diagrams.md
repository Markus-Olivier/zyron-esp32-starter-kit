# Wiring Diagrams

> Zyron ESP32 Starter Kit | [zyron.co.za](https://zyron.co.za)

All diagrams use the **ESP32 DevKit v1** (30-pin or 38-pin). GPIO numbers are the same on both variants.

---

## ESP32 DevKit v1 Pinout Reference

```
               ┌──────────────────┐
          3.3V │ 3V3          GND │ GND
           GND │ GND         IO23 │ 23 (MOSI)
          IO15 │ 15          IO22 │ 22 (SCL / I2C)
           IO2 │ 2 ← LED     IO1  │ TX
           IO4 │ 4           IO3  │ RX
           IO5 │ 5           IO21 │ 21 (SDA / I2C)
          IO18 │ 18 (MISO)   IO19 │ 19
          IO19 │ 19          IO18 │ 18
          IO21 │ 21          IO5  │ 5 (SCK)
          IO22 │ 22          IO17 │ 17
          IO23 │ 23          IO16 │ 16
          IO25 │ 25          IO4  │ 4
          IO26 │ 26          IO0  │ 0
          IO27 │ 27          IO2  │ 2
          IO14 │ 14          IO15 │ 15
          IO12 │ 12          IO8  │ 8
           GND │ GND         IO7  │ 7
          IO13 │ 13          IO6  │ 6
          IO9  │ 9 (flash)   IO11 │ 11
          IO10 │ 10 (flash)  IO10 │ 10
          IO11 │ 11 (flash)  IO9  │ 9
           VIN │ 5V (USB)    GND  │ GND
               └──────────────────┘
```

**ADC-safe pins for WiFi use:** GPIO 34, 35, 36, 39 (ADC1 — always reliable)
**Avoid during WiFi:** GPIO 25–27, 32–33 (ADC2 — unreliable with WiFi active)

---

## Sensor Wiring

### DHT22 Temperature & Humidity Sensor

```
DHT22 Pin │ ESP32 Pin │ Notes
──────────┼───────────┼─────────────────────────────
  VCC     │   3.3V    │ Do NOT use 5V — sensor is 3.3V compatible
  GND     │   GND     │
  DATA    │   GPIO 4  │ + 10kΩ pull-up resistor to 3.3V (required)
  NC      │   -       │ Not connected (4-pin version)
```

**Pull-up circuit:**
```
3.3V ──┬── DHT22 DATA ── GPIO 4
       │
     [10kΩ]
       │
      GND (only if resistor is separate — many modules have it built in)
```

**Common issues:**
- If readings show `nan`, check the pull-up resistor is present.
- Wait 2 seconds after power-on before first read.
- Minimum read interval: 2 seconds.

---

### HC-SR04 Ultrasonic Distance Sensor

> ⚠️ **The ECHO pin outputs 5V. ESP32 GPIO max is 3.3V. Use a voltage divider or level shifter on ECHO.**

```
HC-SR04 Pin │ ESP32 Pin │ Notes
────────────┼───────────┼──────────────────────────────────────────
    VCC     │   5V/VIN  │ Must be 5V — does not work reliably at 3.3V
    GND     │   GND     │
    TRIG    │   GPIO 5  │ Output from ESP32 (3.3V logic is fine for TRIG)
    ECHO    │   GPIO 18 │ Via voltage divider (see below)
```

**Voltage divider for ECHO pin (5V → 3.3V):**
```
ECHO (5V) ─── [1kΩ] ─── GPIO 18 ─── [2kΩ] ─── GND
```
This gives: 5V × 2/(1+2) = 3.33V ✓

**Range:** 2 cm – 400 cm | **Accuracy:** ±3 mm

---

### HC-SR501 PIR Motion Sensor

```
PIR Pin │ ESP32 Pin │ Notes
────────┼───────────┼────────────────────────────────────────────
  VCC   │   5V/VIN  │ 5–20V supply
  GND   │   GND     │
  OUT   │   GPIO 13 │ 3.3V logic output (ESP32-safe, no divider needed)
```

**Onboard potentiometers:**
- **Sensitivity pot** (left): Clockwise = more sensitive (up to ~7m)
- **Time-delay pot** (right): Controls how long OUT stays HIGH after motion (1–300s)

**Jumper settings:**
- **H (repeatable trigger):** OUT stays HIGH while motion continues — recommended
- **L (single trigger):** OUT pulses once per detection event

**Allow 30–60 seconds warm-up time** after power-on before it will trigger.

---

### LDR (Light Dependent Resistor / Photoresistor)

```
         3.3V
          │
        [LDR]     ← resistance changes with light
          │
        GPIO 34   ← analog read here
          │
        [10kΩ]    ← fixed pull-down resistor
          │
         GND
```

**Use GPIO 34–39 (ADC1)** — these work correctly even when WiFi is active.

Typical resistance:
- Bright light: ~100Ω – 1kΩ → high ADC reading
- Dark: ~1MΩ → low ADC reading

---

### SSD1306 OLED Display (0.96" I2C)

```
OLED Pin │ ESP32 Pin │ Notes
─────────┼───────────┼───────────────────────────
  VCC    │   3.3V    │
  GND    │   GND     │
  SCL    │   GPIO 22 │ I2C clock (hardware I2C)
  SDA    │   GPIO 21 │ I2C data  (hardware I2C)
```

**Default I2C address:** `0x3C` (some modules use `0x3D` — check with I2C scanner)

I2C Scanner sketch: [examples/08_oled_sensor_display](../examples/08_oled_sensor_display/)

---

### 5V Relay Module

```
Relay Pin │ ESP32 Pin │ Notes
──────────┼───────────┼────────────────────────────────────────
   VCC    │   5V/VIN  │ Relay coil needs 5V
   GND    │   GND     │
   IN     │   GPIO 26 │ Control signal (LOW = relay ON for most modules)
```

**Load terminals (screw terminals on relay):**
```
COM  → One side of your load (e.g. lamp, fan)
NO   → Other side, connected when relay is activated (Normally Open)
NC   → Other side, connected when relay is NOT activated (Normally Closed)
```

⚠️ **Mains AC safety:** If switching 230V AC, use an appropriately rated relay (10A/250V), ensure all live connections are insulated, and never work on a live circuit. If unsure, use a 12V DC load instead.

---

### SG90 Servo Motor

```
Servo Wire  │ ESP32 Pin │ Notes
────────────┼───────────┼──────────────────────────────────────
  Brown/BLK │   GND     │
  Red        │   5V/VIN  │ Servos need 5V. 3.3V may cause jitter.
  Orange/YLW │  GPIO 13  │ PWM signal (3.3V logic is fine for signal)
```

**Note:** If you have multiple servos or a loaded servo, use an external 5V power supply — the ESP32 USB regulator may not supply enough current.

---

### Active Buzzer

```
Buzzer Pin │ ESP32 Pin │ Notes
───────────┼───────────┼──────────────────────────────────────
    (+)    │   GPIO 25 │ Drive HIGH to beep
    (-)    │   GND     │
```

For a **passive buzzer** (can play tones), use `ledcWriteTone()` — see [example 07](../examples/07_buzzer_melody/).

---

### Push Button

```
                ┌────── GPIO 15 (INPUT_PULLUP)
               [BTN]
                └────── GND
```

Using `INPUT_PULLUP`: pin reads HIGH normally, LOW when button is pressed. No external resistor needed.

---

## Full Kit Multi-Sensor Wiring

For projects that use all sensors simultaneously:

| Component    | VCC    | GND | Signal 1       | Signal 2       |
|---|---|---|---|---|
| DHT22        | 3.3V   | GND | GPIO 4 (DATA)  | —              |
| HC-SR04      | 5V     | GND | GPIO 5 (TRIG)  | GPIO 18 (ECHO) |
| HC-SR501 PIR | 5V     | GND | GPIO 13 (OUT)  | —              |
| LDR          | 3.3V   | GND | GPIO 34 (ADC)  | —              |
| OLED         | 3.3V   | GND | GPIO 22 (SCL)  | GPIO 21 (SDA)  |
| Relay        | 5V     | GND | GPIO 26 (IN)   | —              |
| Servo        | 5V     | GND | GPIO 13 (PWM)  | —              |
| Buzzer       | —      | GND | GPIO 25 (IN)   | —              |
| Button       | —      | GND | GPIO 15 (IN)   | —              |

> Note: PIR and Servo share GPIO 13 in the default pin assignments above. If using both simultaneously, reassign the servo to GPIO 27 and update `SERVO_PIN` in the sketch.
