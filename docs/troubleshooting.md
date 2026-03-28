# Troubleshooting Guide

> Zyron ESP32 Starter Kit | [zyron.co.za](https://zyron.co.za)

---

## Upload / Flashing Issues

### "Failed to connect to ESP32: Timed out waiting for packet header"

**Cause:** The ESP32 is not entering download mode.

**Fix:**
1. Hold the **BOOT** button on the ESP32 while clicking **Upload** in Arduino IDE.
2. Release BOOT once you see `Connecting...` in the output.
3. Or: hold BOOT, press and release EN (reset), then release BOOT.

---

### "No port found" / COM port not listed

**Cause:** USB driver not installed.

**Fix:**
- Most ESP32 DevKit v1 boards use the **CP2102** USB-UART chip.
- Download the driver: Silicon Labs CP210x USB to UART Bridge
- Some boards use **CH340** — search "CH340 driver" for your OS.
- On Windows: Device Manager → Ports (COM & LPT) → Update driver.

---

### Board keeps resetting (brownout detected)

**Cause:** Insufficient power — USB cable or power supply can't deliver enough current.

**Fix:**
- Use a good quality USB cable (not a charge-only cable).
- Avoid powering servos or relays directly from the ESP32's VIN — use a separate 5V supply.
- If using a laptop USB hub, connect directly to the laptop port.

---

## DHT22 Sensor Issues

### Readings show `nan` or `0.0`

**Causes and fixes:**
1. **Missing pull-up resistor** — Add a 10kΩ resistor between the DATA pin and 3.3V.
2. **Wrong pin** — Verify `DHT_PIN` in your sketch matches physical wiring.
3. **Too fast reads** — DHT22 needs at minimum 2 seconds between reads. Check your `delay()`.
4. **Faulty module** — Test with a fresh power cycle; DHT22 can lock up on bad starts.

### "DHT22 read failed" consistently

- Verify you are using `DHT22` (AM2302), not `DHT11` — change the type in `DHT dht(PIN, DHT11)` if needed.
- Try a different GPIO pin. Avoid GPIOs 6–11 (connected to internal flash).

---

## HC-SR04 Distance Sensor

### Always returns -1 or 0 (out of range)

**Causes and fixes:**
1. **ECHO voltage too high** — The ECHO pin outputs 5V. ESP32 GPIO max is 3.3V. Add a voltage divider (1kΩ + 2kΩ) or level shifter.
2. **Powered from 3.3V** — HC-SR04 needs 5V to work. Connect VCC to the VIN/5V pin.
3. **Object too close** — Minimum detection distance is 2 cm.
4. **Object too far or angled** — Maximum reliable range is ~300 cm. Angled surfaces scatter the signal.

### Readings are erratic or jumping

- Hold the sensor steady — vibrations interfere with the echo.
- Avoid soft/sound-absorbing materials (foam, cloth) as targets.
- Ensure TRIG is pulsed correctly: LOW → HIGH (10µs) → LOW.

---

## PIR Motion Sensor (HC-SR501)

### Never triggers / always triggers

**Never triggers:**
- Allow **30–60 seconds warm-up** after power-on — this is normal behaviour.
- Check OUT is wired to the correct GPIO and `pinMode` is `INPUT`.
- Turn the sensitivity potentiometer clockwise to increase range.

**Always stays HIGH:**
- Reduce sensitivity (turn sensitivity pot counter-clockwise).
- Reduce time-delay pot (turn counter-clockwise) to shorten trigger duration.
- Move away from the sensor — the calibration detects the person setting it up.

### Triggers randomly with no movement

- Keep away from AC vents, heaters, or fans — temperature changes trigger PIR.
- Shield from direct sunlight or flickering lights.
- Switch jumper to **H mode** (repeatable trigger) for stable behaviour.

---

## OLED Display (SSD1306)

### Blank / nothing shows

1. **Wrong I2C address** — Most modules are `0x3C`; some are `0x3D`. Run an I2C scanner:
   ```cpp
   #include <Wire.h>
   void setup() {
     Serial.begin(115200);
     Wire.begin();
     for (byte addr = 1; addr < 127; addr++) {
       Wire.beginTransmission(addr);
       if (Wire.endTransmission() == 0)
         Serial.printf("Found I2C device at 0x%02X\n", addr);
     }
   }
   void loop() {}
   ```
2. **SDA/SCL swapped** — GPIO 21 = SDA, GPIO 22 = SCL on most ESP32 DevKits.
3. **Missing `display.display()` call** — Content won't appear until you call this.
4. **Power issue** — Some displays need more than 3.3V can supply; try 5V if available.

---

## WiFi Issues

### Cannot connect to WiFi

- Double-check `WIFI_SSID` and `WIFI_PASS` — they are case-sensitive.
- ESP32 only supports **2.4 GHz** WiFi — 5 GHz networks will not work.
- Try moving closer to the router during testing.
- If connecting to a phone hotspot, ensure the hotspot is on 2.4 GHz band.

### WiFi keeps disconnecting

- Add reconnection logic in `loop()`:
  ```cpp
  if (WiFi.status() != WL_CONNECTED) {
      WiFi.reconnect();
      delay(5000);
  }
  ```
- For production, use `WiFi.setAutoReconnect(true)` and `WiFi.persistent(false)` in `setup()`.

### ADC (analogRead) gives wrong values when WiFi is on

- **Known ESP32 limitation:** ADC2 pins (GPIO 25–27, 32–33) are shared with the WiFi radio and give unreliable readings when WiFi is active.
- **Fix:** Use ADC1 pins instead — GPIO 34, 35, 36, 39 are safe with WiFi active.

---

## MQTT Issues

### Cannot connect to broker

- Verify the broker IP address is correct and the broker is running.
- Test connectivity: `ping 192.168.1.100` from your PC.
- Check firewall — port 1883 must be open.
- For Mosquitto on local network, ensure `listener 1883` and `allow_anonymous true` are in `mosquitto.conf`.

### Messages not arriving in Home Assistant

- Ensure MQTT integration is configured in Home Assistant.
- Use MQTT Explorer (free tool) to verify the ESP32 is publishing to the expected topics.
- Check topic spelling — MQTT topics are case-sensitive.

---

## Deep Sleep

### ESP32 doesn't wake up

- Verify the PIR OUT signal is actually going HIGH (test with regular `digitalRead` first).
- For EXT0 wakeup, the GPIO must be an RTC-capable pin: 0, 2, 4, 12–15, 25–27, 32–39.
- Check that `esp_sleep_enable_ext0_wakeup()` is called before `esp_deep_sleep_start()`.

### RTC_DATA_ATTR variables lose value after sleep

- RTC memory is preserved across deep sleep but NOT across power-off or hard reset.
- If the board loses power completely, `RTC_DATA_ATTR` variables reset to 0.

---

## General Tips

| Symptom | First thing to check |
|---|---|
| Sketch uploads but does nothing | Check `Serial.begin(115200)` and baud rate in monitor |
| Sensor reads 0 constantly | Check VCC connection and correct pin number |
| WiFi connects but HTTP fails | Check firewall, correct IP, server is running |
| Random resets | Power supply — add 100µF capacitor across VIN/GND |
| GPIOs 6–11 not working | These are connected to internal flash — never use them |
| Code compiles but behaviour is wrong | Add `Serial.printf` debug prints to trace execution |

---

## Getting Help

- Open an issue: [github.com/ZyronSoftware/zyron-esp32-starter-kit/issues](https://github.com/ZyronSoftware/zyron-esp32-starter-kit/issues)
- Visit: [zyron.co.za](https://zyron.co.za)
- Follow: [@ZyronSoftware](https://x.com/ZyronSoftware)
