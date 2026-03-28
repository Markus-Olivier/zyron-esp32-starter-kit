# ESP32 Starter Kit — Introduction Course

> **Zyron Software** | [zyron.co.za](https://zyron.co.za) | *We Build What Off-The-Shelf Can't*

This course takes you from zero to building real IoT projects with your ESP32 starter kit. No prior hardware experience needed — just curiosity and the kit in front of you.

---

## Course Modules

| # | Lesson | What You'll Learn |
|---|---|---|
| 01 | [Meet the ESP32](01_meet_the_esp32.md) | What the ESP32 is, why it's special, board layout and pins |
| 02 | [Your Kit Components](02_kit_components.md) | Every component in the kit, what it looks like and what it does |
| 02b | [How to Use a Breadboard](02b_breadboard.md) | Internal connections, power rails, LED circuit, common mistakes |
| 03 | [DHT22 — Temperature & Humidity](03_dht22_sensor.md) | How it works, wiring, reading data, troubleshooting |
| 04 | [HC-SR04 — Ultrasonic Distance](04_hcsr04_sensor.md) | Sound waves, timing, voltage safety, range limits |
| 05 | [HC-SR501 — PIR Motion](05_pir_sensor.md) | Infrared detection, calibration pots, warm-up time |
| 06 | [LDR — Light Sensor](06_ldr_sensor.md) | Resistance vs light, voltage dividers, threshold logic |
| 07 | [SSD1306 — OLED Display](07_oled_display.md) | I2C protocol, drawing text and graphics, I2C scanner |
| 08 | [Relay Module](08_relay_module.md) | Switching loads, NO/NC contacts, mains safety |
| 09 | [SG90 — Servo Motor](09_servo_motor.md) | PWM control, angle mapping, current requirements |
| 10 | [Buzzer & Button](10_buzzer_button.md) | Active vs passive buzzers, tones, debouncing |

---

## Before You Start

### You'll need
- Your ESP32 starter kit
- A computer with **Arduino IDE 2.x** installed
- A Micro-USB cable (data cable, not charge-only)
- 15–20 minutes per lesson

### Install Arduino IDE + ESP32 Support

1. Download Arduino IDE 2.x from [arduino.cc/en/software](https://www.arduino.cc/en/software)
2. Open Arduino IDE → **File → Preferences**
3. In *Additional board manager URLs*, paste:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Go to **Tools → Board → Boards Manager** → search `esp32` → install **esp32 by Espressif Systems**
5. Select your board: **Tools → Board → ESP32 Arduino → ESP32 Dev Module**

### Install Required Libraries

**Tools → Manage Libraries** — install each of these:

| Library | Author |
|---|---|
| DHT sensor library | Adafruit |
| Adafruit Unified Sensor | Adafruit |
| Adafruit SSD1306 | Adafruit |
| Adafruit GFX Library | Adafruit |
| PubSubClient | Nick O'Leary |
| ArduinoJson | Benoit Blanchon |
| ESP32Servo | Kevin Harrington |

---

## How to Flash Your First Sketch

1. Connect your ESP32 to your computer via Micro-USB
2. Go to **Tools → Port** and select the COM port for your ESP32
   - Windows: `COM3`, `COM4`, etc.
   - macOS/Linux: `/dev/ttyUSB0` or `/dev/ttyACM0`
3. Open any `.ino` example from the `examples/` folder
4. Click the **Upload** button (→ arrow)
5. If it fails, hold the **BOOT** button on the ESP32 while clicking Upload

> **Can't find the COM port?** Install the CP2102 USB driver for your OS — see [troubleshooting guide](../troubleshooting.md).

---

## Safety Rules

Before touching any wiring:

- **Always unplug the ESP32** from USB before changing wires on a breadboard
- **Never connect 5V to a GPIO input** without a voltage divider — ESP32 GPIO pins are 3.3V maximum
- **The HC-SR04 ECHO pin outputs 5V** — always use a voltage divider (see [Lesson 04](04_hcsr04_sensor.md))
- **Relay + mains AC is dangerous** — if you are not electrically qualified, only switch low-voltage DC loads (LEDs, fans, pumps ≤ 12V DC)
- When in doubt, ask first

---

*Built by [Zyron Software](https://zyron.co.za) — South African custom software engineering for robotics, IoT, and smart systems.*
