# ⚡ Zyron ESP32 Starter Kit

> **Built by [Zyron Software](https://zyron.co.za) — We Build What Off-The-Shelf Can't**

A complete, production-ready ESP32 project library for IoT, home automation, and remote monitoring. **20 fully-working Arduino examples** covering everything from a basic LED blink to a full WiFi weather station — all using the components in your ESP32 basic starter kit.

Whether you're a beginner learning Arduino for the first time, or a developer building production IoT devices, start here.

[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![Platform: ESP32](https://img.shields.io/badge/Platform-ESP32-blue.svg)](https://www.espressif.com/en/products/socs/esp32)
[![Made in South Africa](https://img.shields.io/badge/Made%20in-South%20Africa-gold.svg)](https://zyron.co.za)

---

## 🎓 New to ESP32? Start with the Course

Not sure what the DHT22 does or how to wire anything up? The **Introduction Course** explains every component visually with diagrams, photos, and working code examples — no prior hardware experience needed.

| Lesson | Topic |
|---|---|
| [01 — Meet the ESP32](docs/course/01_meet_the_esp32.md) | Board layout, GPIO pins, how code runs |
| [02 — Your Kit Components](docs/course/02_kit_components.md) | Every component identified with photos |
| [03 — DHT22](docs/course/03_dht22_sensor.md) | Temperature & humidity — how it works, wiring, code |
| [04 — HC-SR04](docs/course/04_hcsr04_sensor.md) | Ultrasonic distance — sound timing, 5V safety |
| [05 — PIR Sensor](docs/course/05_pir_sensor.md) | Motion detection — calibration, pots, trigger modes |
| [06 — LDR](docs/course/06_ldr_sensor.md) | Light sensor — voltage dividers, threshold logic |
| [07 — OLED Display](docs/course/07_oled_display.md) | I2C, drawing text and graphics |
| [08 — Relay Module](docs/course/08_relay_module.md) | Switching loads, NO/NC contacts, safety |
| [09 — Servo Motor](docs/course/09_servo_motor.md) | PWM control, angle mapping |
| [10 — Buzzer & Button](docs/course/10_buzzer_button.md) | Tones, melodies, debouncing |

➡️ **[Open the full course →](docs/course/README.md)**

---

## 📦 20 ESP32 Projects

### Beginner — No WiFi Required

| # | Project | Components | Skills |
|---|---|---|---|
| 01 | [Blink](examples/01_blink/) | LED | GPIO output, timing |
| 02 | [Button + LED](examples/02_button_led/) | LED, push button | Digital input, debouncing |
| 03 | [DHT22 Monitor](examples/03_dht22_monitor/) | DHT22 | Sensor reading, serial output |
| 04 | [Ultrasonic Distance](examples/04_ultrasonic_distance/) | HC-SR04 | pulseIn, distance calc |
| 05 | [PIR Motion Alarm](examples/05_pir_motion_alarm/) | PIR, buzzer, LED | Interrupts, alarm state machine |
| 06 | [LDR Auto Night Light](examples/06_ldr_night_light/) | LDR, relay | analogRead, hysteresis control |
| 07 | [Buzzer Melody](examples/07_buzzer_melody/) | Buzzer | LEDC PWM, tone arrays |
| 08 | [OLED Sensor Display](examples/08_oled_sensor_display/) | OLED, DHT22, HC-SR04 | I2C, multi-sensor display |

### Intermediate — WiFi & Connectivity

| # | Project | Components | Skills |
|---|---|---|---|
| 09 | [Relay Web Control](examples/09_relay_web_control/) | Relay | HTTP server, HTML, remote control |
| 10 | [Servo Control](examples/10_servo_sweep/) | Servo, potentiometer | PWM servo, analog mapping |
| 11 | [WiFi Sensor Dashboard](examples/11_wifi_sensor_dashboard/) | DHT22, HC-SR04, PIR | Web dashboard, JSON API |
| 12 | [MQTT Publisher](examples/12_mqtt_publisher/) | DHT22, HC-SR04, PIR | MQTT, Home Assistant, reconnect |
| 13 | [REST API Client](examples/13_rest_api_client/) | DHT22, HC-SR04, PIR | HTTPClient, JSON POST |
| 14 | [BLE Presence Detector](examples/14_ble_scanner/) | None (built-in BT) | BLE scan, MAC filtering, RSSI |
| 15 | [NTP Clock + OLED](examples/15_ntp_clock_display/) | OLED | NTP sync, time formatting, display |

### Advanced — Production IoT

| # | Project | Components | Skills |
|---|---|---|---|
| 16 | [Deep Sleep Battery Monitor](examples/16_deep_sleep_monitor/) | DHT22, PIR | Deep sleep, RTC memory, ~10µA idle |
| 17 | [OTA Firmware Update](examples/17_ota_update/) | LED | ArduinoOTA, mDNS, wireless flash |
| 18 | [SPIFFS Data Logger](examples/18_spiffs_data_logger/) | DHT22 | Flash filesystem, CSV logging, download |
| 19 | [Smart Home Node](examples/19_smart_home_node/) | DHT22, PIR, LDR, relay | Multi-sensor fusion, MQTT, automation |
| 20 | [Full Weather Station](examples/20_weather_station/) | DHT22, LDR, OLED | System integration, full-stack IoT |

---

## 🛒 Hardware You'll Need

### Core Board
- **ESP32 DevKit v1** (38-pin or 30-pin) — available from [amazon.com](https://www.amazon.com) or similar

### Sensors & Components

| Component | Used In | Notes |
|---|---|---|
| **DHT22** (AM2302) | 03, 08, 11–13, 16–20 | Temperature & humidity |
| **HC-SR04** | 04, 08, 11–13 | Ultrasonic distance sensor |
| **HC-SR501** (PIR) | 05, 11–13, 16, 19 | Passive infrared motion |
| **LDR** (photoresistor) | 06, 19, 20 | Ambient light level |
| **SSD1306 OLED** (0.96") | 08, 15, 20 | I2C display |
| **5V Relay module** | 06, 09, 19 | Switch loads up to 10A/250V |
| **SG90 Servo** | 10 | 0–180° PWM motor |
| **Active Buzzer** | 05, 07 | Alarms and tones |
| **Push button** | 02 | Digital input |
| **LED** (+ 220Ω) | 01, 02, 05 | Output indicator |
| **Potentiometer** | 10 | Analog input for servo control |
| **Breadboard + jumper wires** | All | Prototyping |
| **10kΩ resistors** | 03, 06 | DHT22 pull-up, LDR divider |

---

## 🔌 Wiring Quick Reference

| Component | VCC | GND | Signal Pins |
|---|---|---|---|
| DHT22 | 3.3V | GND | GPIO 4 (DATA + 10kΩ pull-up) |
| HC-SR04 | 5V | GND | GPIO 5 (TRIG), GPIO 18 (ECHO via voltage divider) |
| PIR HC-SR501 | 5V | GND | GPIO 13 (OUT) |
| LDR | 3.3V | GND | GPIO 34 (ADC, voltage divider) |
| OLED SSD1306 | 3.3V | GND | GPIO 22 (SCL), GPIO 21 (SDA) |
| Relay | 5V | GND | GPIO 26 (IN) |
| Servo SG90 | 5V | GND | GPIO 13 (PWM signal) |
| Buzzer | — | GND | GPIO 25 |
| Button | — | GND | GPIO 15 (INPUT_PULLUP) |

> Full diagrams with voltage divider circuits → [docs/wiring-diagrams.md](docs/wiring-diagrams.md)

> ⚠️ **HC-SR04 ECHO is 5V** — always use a voltage divider or level shifter. **ESP32 GPIO max is 3.3V.**

---

## 🚀 Quick Start

### 1. Install Arduino IDE 2.x + ESP32 Support

Add the ESP32 board manager URL:
```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```
Go to: **Tools → Board → Boards Manager** → search `esp32` → install by Espressif

### 2. Install Required Libraries

In **Tools → Manage Libraries**, install:

| Library | Author | Used by |
|---|---|---|
| `DHT sensor library` | Adafruit | 03, 08, 11–13, 16–20 |
| `Adafruit Unified Sensor` | Adafruit | (dependency of above) |
| `Adafruit SSD1306` | Adafruit | 08, 15, 20 |
| `Adafruit GFX Library` | Adafruit | (dependency of above) |
| `PubSubClient` | Nick O'Leary | 12, 19, 20 |
| `ArduinoJson` | Benoit Blanchon | 11–13, 16, 18–20 |
| `ESP32Servo` | Kevin Harrington | 10 |

### 3. Flash an Example

1. Open any `.ino` file from the `examples/` folder
2. Edit `WIFI_SSID` and `WIFI_PASS` at the top (for WiFi examples)
3. Select **Tools → Board → ESP32 Dev Module**
4. Select the correct COM port
5. Click **Upload**

---

## 📁 Repository Structure

```
zyron-esp32-starter-kit/
├── README.md
├── LICENSE
├── library.properties
├── src/
│   ├── ZyronSensors.h          ← Sensor abstraction library
│   └── ZyronSensors.cpp
├── examples/
│   ├── 01_blink/
│   ├── 02_button_led/
│   ├── 03_dht22_monitor/
│   ├── 04_ultrasonic_distance/
│   ├── 05_pir_motion_alarm/
│   ├── 06_ldr_night_light/
│   ├── 07_buzzer_melody/
│   ├── 08_oled_sensor_display/
│   ├── 09_relay_web_control/
│   ├── 10_servo_sweep/
│   ├── 11_wifi_sensor_dashboard/
│   ├── 12_mqtt_publisher/
│   ├── 13_rest_api_client/
│   ├── 14_ble_scanner/
│   ├── 15_ntp_clock_display/
│   ├── 16_deep_sleep_monitor/
│   ├── 17_ota_update/
│   ├── 18_spiffs_data_logger/
│   ├── 19_smart_home_node/
│   └── 20_weather_station/
├── images/
│   ├── components/              ← component photos for course
│   └── wiring/                  ← wiring diagram photos
└── docs/
    ├── wiring-diagrams.md
    ├── troubleshooting.md
    └── course/
        ├── README.md            ← course index
        ├── 01_meet_the_esp32.md
        ├── 02_kit_components.md
        ├── 03_dht22_sensor.md
        ├── 04_hcsr04_sensor.md
        ├── 05_pir_sensor.md
        ├── 06_ldr_sensor.md
        ├── 07_oled_display.md
        ├── 08_relay_module.md
        ├── 09_servo_motor.md
        └── 10_buzzer_button.md
```

---

## 🔧 The ZyronSensors Library

The `src/` folder contains a lightweight C++ wrapper around DHT22, HC-SR04, and HC-SR501 so your sketches stay clean:

```cpp
#include "ZyronSensors.h"

ZyronDHT         climate(4);           // DHT22 on GPIO 4
ZyronUltrasonic  sonar(5, 18);         // TRIG=5, ECHO=18
ZyronPIR         pir(13);             // PIR on GPIO 13
ZyronLDR         light(34);            // LDR on GPIO 34

void setup() {
    climate.begin();
    sonar.begin();
    pir.begin();
    light.begin();
}

void loop() {
    TempHumidity env = climate.read();
    if (env.valid) Serial.printf("%.1f°C  %.1f%%\n", env.temperatureC, env.humidity);

    float dist = sonar.distanceCm();
    Serial.printf("%.1f cm\n", dist);

    if (pir.motionDetectedDebounced()) Serial.println("Motion!");

    Serial.printf("Light: %d%%  Dark: %s\n", light.percentage(), light.isDark() ? "yes" : "no");
    delay(1000);
}
```

---

## 💡 Project Ideas

Once you've run the examples, combine them to build:

- **Security camera node** — PIR wakeup + photo (with ESP32-CAM module) + MQTT alert
- **Greenhouse monitor** — DHT22 + LDR + relay-controlled fan/heater + Home Assistant
- **Smart parking sensor** — HC-SR04 + LED indicator + MQTT to dashboard
- **Baby room monitor** — Temperature + humidity + motion + OLED + phone notification via MQTT
- **Water tank level monitor** — HC-SR04 pointing down into tank + REST API + web dashboard
- **Presence-based lighting** — BLE phone detection + relay + MQTT automations

---

## 🤝 Contributing

PRs welcome. Fork → branch → commit → PR.

- Stick to the existing naming convention (`NN_project_name`)
- Each sketch must be self-contained with full wiring in the header comment
- Test on real hardware before submitting

---

## 📖 Documentation

- [Introduction Course](docs/course/README.md) — 10-lesson beginner course with component photos and wiring
- [Wiring Diagrams](docs/wiring-diagrams.md) — detailed pin diagrams for every component
- [Troubleshooting Guide](docs/troubleshooting.md) — fixes for the most common issues

---

## 📄 License

MIT — free to use in personal and commercial projects. See [LICENSE](LICENSE).

---

## 🏢 About Zyron Software

[Zyron Software](https://zyron.co.za) is a South African custom software engineering company specialising in **robotics-web integration**, **Arduino & ESP32 APIs**, and **IoT dashboards**.

We design, build, and deploy complete IoT systems — from embedded firmware to cloud dashboards — for businesses across South Africa and beyond.

> *We Build What Off-The-Shelf Can't*

🌐 [zyron.co.za](https://zyron.co.za) · 🐦 [@ZyronSoftware](https://x.com/ZyronSoftware)

*If this helped you build something cool, drop us a ⭐*
