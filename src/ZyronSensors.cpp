/*
 * ZyronSensors.cpp
 * Zyron ESP32 Starter Kit — Sensor Abstraction Library
 *
 * https://zyron.co.za | MIT License
 */

#include "ZyronSensors.h"

// ─── ZyronDHT ─────────────────────────────────────────────────────────────────

ZyronDHT::ZyronDHT(uint8_t pin, uint8_t type) : _dht(pin, type) {}

void ZyronDHT::begin() {
    _dht.begin();
}

TempHumidity ZyronDHT::read() {
    TempHumidity result;
    result.humidity      = _dht.readHumidity();
    result.temperatureC  = _dht.readTemperature();
    result.temperatureF  = _dht.readTemperature(true);

    if (isnan(result.humidity) || isnan(result.temperatureC)) {
        result.valid = false;
        result.heatIndexC = 0.0f;
        return result;
    }

    result.heatIndexC = _dht.computeHeatIndex(result.temperatureC, result.humidity, false);
    result.valid = true;
    return result;
}

// ─── ZyronUltrasonic ──────────────────────────────────────────────────────────

ZyronUltrasonic::ZyronUltrasonic(uint8_t trigPin, uint8_t echoPin)
    : _trig(trigPin), _echo(echoPin) {}

void ZyronUltrasonic::begin() {
    pinMode(_trig, OUTPUT);
    pinMode(_echo, INPUT);
    digitalWrite(_trig, LOW);
}

float ZyronUltrasonic::_measureCm() {
    digitalWrite(_trig, LOW);
    delayMicroseconds(2);
    digitalWrite(_trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(_trig, LOW);

    long duration = pulseIn(_echo, HIGH, 30000UL); // 30 ms timeout (~5 m)
    if (duration == 0) return -1.0f;               // out of range
    return duration * 0.034f / 2.0f;
}

float ZyronUltrasonic::distanceCm() {
    return _measureCm();
}

float ZyronUltrasonic::distanceInch() {
    float cm = _measureCm();
    return (cm < 0) ? -1.0f : cm / 2.54f;
}

// ─── ZyronPIR ─────────────────────────────────────────────────────────────────

ZyronPIR::ZyronPIR(uint8_t pin) : _pin(pin), _lastTrigger(0) {}

void ZyronPIR::begin() {
    pinMode(_pin, INPUT);
}

bool ZyronPIR::motionDetected() {
    return digitalRead(_pin) == HIGH;
}

bool ZyronPIR::motionDetectedDebounced(uint32_t debounceMs) {
    if (digitalRead(_pin) == HIGH) {
        uint32_t now = millis();
        if (now - _lastTrigger >= debounceMs) {
            _lastTrigger = now;
            return true;
        }
    }
    return false;
}

// ─── ZyronLDR ─────────────────────────────────────────────────────────────────

ZyronLDR::ZyronLDR(uint8_t analogPin) : _pin(analogPin) {}

void ZyronLDR::begin() {
    pinMode(_pin, INPUT);
}

int ZyronLDR::rawValue() {
    return analogRead(_pin);
}

float ZyronLDR::voltage() {
    return rawValue() * (3.3f / 4095.0f);
}

uint8_t ZyronLDR::percentage() {
    return map(rawValue(), 0, 4095, 0, 100);
}

bool ZyronLDR::isDark(int threshold) {
    return rawValue() < threshold;
}
