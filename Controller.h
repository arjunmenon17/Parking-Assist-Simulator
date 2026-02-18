#pragma once
#include <Arduino.h>

enum class AssistState : uint8_t {
  INVALID = 0,
  SAFE,
  CAUTION,
  DANGER
};

const char* toString(AssistState s);

class ParkAssistController {
public:
  ParkAssistController(int dangerCm, int cautionCm, int minValidCm, int maxValidCm, int hysteresisCm);

  void update(int distanceCm, bool valid);
  AssistState state() const { return state_; }

private:
  AssistState classify(int distanceCm) const;

  int dangerCm_;
  int cautionCm_;
  int minValidCm_;
  int maxValidCm_;
  int hys_;

  AssistState state_;
};