#pragma once

/*
 * ZyronSensors.h
 * Zyron ESP32 Starter Kit — Sensor Abstraction Library
 *
 * https://zyron.co.za | MIT License
 *
 * Wraps DHT22, HC-SR04, and HC-SR501 PIR with a clean,
 * consistent API so examples stay readable.
 */

#include <Arduino.h>
#include <DHT.h>

// ─── DHT22 ────────────────────────────────────────────────────────────────────

struct TempHumidity {
    float temperatureC;
    float temperatureF;
    float humidity;
    float heatIndexC;
    bool  valid;
};

class ZyronDHT {
public:
    ZyronDHT(uint8_t pin, uint8_t type = DHT22);
    void        begin();
    TempHumidity read();

private:
    DHT _dht;
};

// ─── HC-SR04 Ultrasonic ───────────────────────────────────────────────────────

class ZyronUltrasonic {
public:
    ZyronUltrasonic(uint8_t trigPin, uint8_t echoPin);
    void  begin();
    float distanceCm();
    float distanceInch();

private:
    uint8_t _trig;
    uint8_t _echo;
    float   _measureCm();
};

// ─── HC-SR501 PIR ─────────────────────────────────────────────────────────────

class ZyronPIR {
public:
    ZyronPIR(uint8_t pin);
    void begin();
    bool motionDetected();
    bool motionDetectedDebounced(uint32_t debounceMs = 500);

private:
    uint8_t  _pin;
    uint32_t _lastTrigger;
};

// ─── LDR (Light Dependent Resistor) ──────────────────────────────────────────

class ZyronLDR {
public:
    ZyronLDR(uint8_t analogPin);
    void    begin();
    int     rawValue();       // 0–4095 (12-bit ADC)
    float   voltage();        // 0.0–3.3V
    uint8_t percentage();     // 0 = dark, 100 = bright
    bool    isDark(int threshold = 1000);

private:
    uint8_t _pin;
};
