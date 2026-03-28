# Images

This folder holds all photos referenced throughout the course and documentation.

---

## How to Add Photos

Take clear, well-lit photos of each component against a neutral background (white paper or grey mat works well). Natural light or a softbox gives the best results. Aim for **800×600 px minimum**, **JPEG or PNG**.

Name files exactly as listed below — the course markdown files already reference these paths.

---

## Required Photos

### components/
Photos of individual components, alone, clearly labelled:

| Filename | Subject | Notes |
|---|---|---|
| `components/esp32_devkit.jpg` | ESP32 DevKit v1 board | Top-down shot showing all pins |
| `components/dht22.jpg` | DHT22 sensor | Show the mesh grill face |
| `components/hcsr04.jpg` | HC-SR04 sensor | Front-on showing both cylinders |
| `components/hcsr501.jpg` | HC-SR501 PIR | Show the white dome from above and below |
| `components/hcsr501_controls.jpg` | PIR PCB (dome removed) | Show S1, S2 pots and jumper clearly |
| `components/ldr.jpg` | LDR photoresistor | Close-up showing the squiggly element |
| `components/oled_ssd1306.jpg` | SSD1306 OLED module | Display face showing screen area |
| `components/relay_module.jpg` | 5V relay module | Show both control pins and screw terminals |
| `components/sg90_servo.jpg` | SG90 servo with horns | Show output shaft and 3-wire cable |
| `components/buzzer.jpg` | Active buzzer | Show polarity marking |
| `components/push_button.jpg` | Tactile push button | Show 4-leg layout |
| `components/resistors.jpg` | Assorted resistors | Fan them out to show colour bands |
| `components/breadboard.jpg` | Breadboard | Angled shot showing holes and rails |
| `components/jumper_wires.jpg` | Jumper wire pack | Male-male, male-female, female-female |

### wiring/
Wiring diagrams — either photos of actual breadboard builds or Fritzing diagrams exported as images:

| Filename | Subject |
|---|---|
| `wiring/dht22_wiring.jpg` | DHT22 wired to ESP32 on breadboard |
| `wiring/hcsr04_wiring.jpg` | HC-SR04 with voltage divider on breadboard |
| `wiring/pir_wiring.jpg` | PIR sensor wired to ESP32 |
| `wiring/ldr_wiring.jpg` | LDR voltage divider circuit |
| `wiring/oled_wiring.jpg` | OLED connected via I2C |
| `wiring/relay_wiring.jpg` | Relay module with LED load |
| `wiring/servo_wiring.jpg` | Servo connected to ESP32 |

### (root images/)
| Filename | Subject |
|---|---|
| `esp32_devkit.jpg` | Hero shot of the ESP32 board (used in Lesson 01) |
| `esp32_board_annotated.jpg` | ESP32 board with labels: USB, antenna, buttons, pins |

---

## Tools for Wiring Diagrams

If you prefer software diagrams over breadboard photos:

- **[Fritzing](https://fritzing.org)** — free circuit diagram tool with ESP32 and most sensors in its library. Export as PNG or JPEG.
- **[Wokwi](https://wokwi.com)** — browser-based ESP32 simulator with screenshot export. You can also share a live simulation link.
- **[draw.io / diagrams.net](https://app.diagrams.net)** — general diagramming tool

---

## Folder Structure

```
images/
├── README.md           ← this file
├── esp32_devkit.jpg
├── esp32_board_annotated.jpg
├── components/
│   ├── esp32_devkit.jpg
│   ├── dht22.jpg
│   ├── hcsr04.jpg
│   ├── hcsr501.jpg
│   ├── hcsr501_controls.jpg
│   ├── ldr.jpg
│   ├── oled_ssd1306.jpg
│   ├── relay_module.jpg
│   ├── sg90_servo.jpg
│   ├── buzzer.jpg
│   ├── push_button.jpg
│   ├── resistors.jpg
│   ├── breadboard.jpg
│   └── jumper_wires.jpg
└── wiring/
    ├── dht22_wiring.jpg
    ├── hcsr04_wiring.jpg
    ├── pir_wiring.jpg
    ├── ldr_wiring.jpg
    ├── oled_wiring.jpg
    ├── relay_wiring.jpg
    └── servo_wiring.jpg
```

---

*[Zyron Software](https://zyron.co.za) — We Build What Off-The-Shelf Can't*
