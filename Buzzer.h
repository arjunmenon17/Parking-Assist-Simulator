#pragma once
#include <Arduino.h>
#include "Controller.h"

class Buzzer {
public:
  explicit Buzzer(uint8_t pin) : pin_(pin) {}

  void begin() {
    pinMode(pin_, OUTPUT);
    stop();
  }

  // Call this every loop (or at least frequently)
  // distanceCm = filtered distance (ignored if invalid)
  void update(uint32_t nowMs, AssistState state, int distanceCm, bool valid) {
    if (!valid || state == AssistState::INVALID || state == AssistState::SAFE) {
      stop();
      return;
    }

    // CAUTION and DANGER patterns.
    // For passive buzzer: we "beep" by turning tone on/off with a cadence.
    const uint16_t freqHz = 2048; // your buzzer's rated frequency

    // If very close, continuous tone.
    if (state == AssistState::DANGER && distanceCm <= 10) {
      startTone(freqHz);
      return;
    }

    uint32_t periodMs = cadencePeriodMs(state, distanceCm);
    uint32_t onMs     = periodMs / 2; // 50% duty cycle beep

    // Cadence toggle logic (non-blocking)
    if (nowMs - lastToggleMs_ >= (isOn_ ? onMs : (periodMs - onMs))) {
      lastToggleMs_ = nowMs;
      isOn_ = !isOn_;

      if (isOn_) startTone(freqHz);
      else       stopTone();
    }
  }

  void stop() {
    isOn_ = false;
    lastToggleMs_ = 0;
    stopTone();
  }

private:
  uint8_t pin_;
  bool isOn_ = false;
  uint32_t lastToggleMs_ = 0;
  bool toneActive_ = false;

  void startTone(uint16_t hz) {
    if (!toneActive_) {
      tone(pin_, hz);
      toneActive_ = true;
    }
  }

  void stopTone() {
    if (toneActive_) {
      noTone(pin_);
      toneActive_ = false;
    }
  }

  // Map distance -> beep period (ms). Smaller distance => faster beeps.
  uint32_t cadencePeriodMs(AssistState s, int d) const {
    if (s == AssistState::CAUTION) {
      // 60cm -> slow, 30cm -> faster
      // clamp to [30,60]
      if (d < 30) d = 30;
      if (d > 60) d = 60;

      // Linear map: 60cm => 900ms, 30cm => 450ms
      // slope = (450-900)/(30-60) = 15 ms/cm
      return 900 - (60 - d) * 15;
    }

    // DANGER: 30cm -> fast, 10cm -> very fast
    if (d < 10) d = 10;
    if (d > 30) d = 30;

    // Linear map: 30cm => 350ms, 10cm => 120ms
    // slope = (120-350)/(10-30) = 11.5 ms/cm
    return 350 - (30 - d) * 11.5;
  }
};