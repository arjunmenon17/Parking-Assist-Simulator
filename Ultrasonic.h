#pragma once
#include <Arduino.h>

struct DistanceReading {
  bool valid;
  float cm;
};

class UltrasonicSensor {
public:
  UltrasonicSensor(uint8_t trigPin, uint8_t echoPin)
    : trig_(trigPin), echo_(echoPin) {}

  void begin() {
    pinMode(trig_, OUTPUT);
    pinMode(echo_, INPUT);
    digitalWrite(trig_, LOW);
  }

  DistanceReading readDistanceCm() {
    // Trigger pulse
    digitalWrite(trig_, LOW);
    delayMicroseconds(2);
    digitalWrite(trig_, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig_, LOW);

    // Measure echo pulse width (us)
    // 50ms timeout covers HC-SR04 max range comfortably
    unsigned long us = pulseIn(echo_, HIGH, 50000UL);
    if (us == 0) return {false, 0.0f};

    // Convert microseconds to cm:
    // speed of sound ~ 0.0343 cm/us; divide by 2 for round-trip
    float cm = (float)us * 0.0343f / 2.0f;
    return {true, cm};
  }

private:
  uint8_t trig_;
  uint8_t echo_;
};