/*
 * 08 — OLED Sensor Display
 * Zyron ESP32 Starter Kit | https://zyron.co.za
 *
 * Display live temperature, humidity, and distance readings on a
 * 0.96" SSD1306 OLED display using I2C.
 *
 * Wiring:
 *   OLED VCC  → 3.3V
 *   OLED GND  → GND
 *   OLED SCL  → GPIO 22
 *   OLED SDA  → GPIO 21
 *
 *   DHT22 DATA → GPIO 4 (+ 10kΩ pull-up to 3.3V)
 *   HC-SR04 TRIG → GPIO 5
 *   HC-SR04 ECHO → GPIO 18 (voltage divider!)
 *
 * Required Libraries:
 *   - Adafruit SSD1306
 *   - Adafruit GFX Library
 *   - DHT sensor library (Adafruit)
 *
 * Skills: I2C display, multi-sensor integration, layout design
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT  64
#define OLED_RESET     -1
#define OLED_ADDRESS  0x3C

#define DHT_PIN   4
#define DHT_TYPE  DHT22
#define TRIG_PIN  5
#define ECHO_PIN  18

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DHT dht(DHT_PIN, DHT_TYPE);

float measureDistanceCm() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    long dur = pulseIn(ECHO_PIN, HIGH, 30000UL);
    return (dur == 0) ? -1.0f : dur * 0.034f / 2.0f;
}

void setup() {
    Serial.begin(115200);
    dht.begin();

    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
        Serial.println("[ERROR] OLED not found. Check wiring and I2C address.");
        while (true);
    }

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Zyron ESP32 Kit");
    display.println("Sensor Display v1.0");
    display.display();
    delay(2000);
}

void loop() {
    float temp  = dht.readTemperature();
    float hum   = dht.readHumidity();
    float dist  = measureDistanceCm();

    display.clearDisplay();

    // Header
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("-- Zyron Sensor Hub --");

    // Temperature
    display.setCursor(0, 14);
    display.print("Temp: ");
    if (isnan(temp)) {
        display.print("ERR");
    } else {
        display.printf("%.1f C", temp);
    }

    // Humidity
    display.setCursor(0, 26);
    display.print("Hum:  ");
    if (isnan(hum)) {
        display.print("ERR");
    } else {
        display.printf("%.1f %%", hum);
    }

    // Distance
    display.setCursor(0, 38);
    display.print("Dist: ");
    if (dist < 0) {
        display.print("Out of range");
    } else {
        display.printf("%.1f cm", dist);
    }

    // Footer
    display.setCursor(0, 56);
    display.printf("zyron.co.za  %lus", millis() / 1000);

    display.display();
    delay(1000);
}
