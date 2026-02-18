#pragma once
#include <Arduino.h>
#include "Controller.h"

class LedOutputs {
public:
  LedOutputs(uint8_t greenPin, uint8_t yellowPin, uint8_t redPin)
    : g_(greenPin), y_(yellowPin), r_(redPin) {}

  void begin() {
    pinMode(g_, OUTPUT);
    pinMode(y_, OUTPUT);
    pinMode(r_, OUTPUT);
    allOff();
  }

  void apply(AssistState s) {
    switch (s) {
      case AssistState::SAFE:    set(true,  false, false); break;
      case AssistState::CAUTION: set(false, true,  false); break;
      case AssistState::DANGER:  set(false, false, true ); break;
      case AssistState::INVALID: default:
        // INVALID: all off (you could also blink later)
        allOff();
        break;
    }
  }

private:
  void set(bool g, bool y, bool r) {
    digitalWrite(g_, g ? HIGH : LOW);
    digitalWrite(y_, y ? HIGH : LOW);
    digitalWrite(r_, r ? HIGH : LOW);
  }

  void allOff() { set(false, false, false); }

  uint8_t g_, y_, r_;
};