/*
 * 03 — DHT22 Temperature & Humidity Monitor
 * Zyron ESP32 Starter Kit | https://zyron.co.za
 *
 * Read temperature and humidity from a DHT22 sensor every 2 seconds
 * and print the results to the Serial Monitor.
 *
 * Wiring:
 *   DHT22 VCC  → 3.3V
 *   DHT22 GND  → GND
 *   DHT22 DATA → GPIO 4  (+ 10kΩ pull-up resistor to 3.3V)
 *
 * Required Libraries:
 *   - DHT sensor library (Adafruit)
 *   - Adafruit Unified Sensor
 *
 * Skills: I2C sensor reading, error handling, heat index calculation
 */

#include <DHT.h>

#define DHT_PIN  4
#define DHT_TYPE DHT22

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
    Serial.begin(115200);
    dht.begin();
    Serial.println("Zyron ESP32 Starter Kit — 03 DHT22 Monitor");
    Serial.println("-------------------------------------------");
    delay(2000); // DHT22 needs ~2 s to stabilise after power-on
}

void loop() {
    float humidity    = dht.readHumidity();
    float tempC       = dht.readTemperature();
    float tempF       = dht.readTemperature(true);

    if (isnan(humidity) || isnan(tempC)) {
        Serial.println("[ERROR] Failed to read from DHT22. Check wiring.");
        delay(2000);
        return;
    }

    float heatIndexC = dht.computeHeatIndex(tempC, humidity, false);
    float heatIndexF = dht.computeHeatIndex(tempF, humidity);

    Serial.printf("Humidity:    %.1f %%\n",    humidity);
    Serial.printf("Temperature: %.1f °C  /  %.1f °F\n", tempC, tempF);
    Serial.printf("Heat Index:  %.1f °C  /  %.1f °F\n", heatIndexC, heatIndexF);
    Serial.println("-------------------------------------------");

    delay(2000);
}
