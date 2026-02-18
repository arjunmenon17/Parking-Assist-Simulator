#include "Controller.h"

const char* toString(AssistState s) {
  switch (s) {
    case AssistState::INVALID: return "INVALID";
    case AssistState::SAFE:    return "SAFE";
    case AssistState::CAUTION: return "CAUTION";
    case AssistState::DANGER:  return "DANGER";
    default: return "UNKNOWN";
  }
}

ParkAssistController::ParkAssistController(int dangerCm, int cautionCm, int minValidCm, int maxValidCm, int hysteresisCm)
  : dangerCm_(dangerCm),
    cautionCm_(cautionCm),
    minValidCm_(minValidCm),
    maxValidCm_(maxValidCm),
    hys_(hysteresisCm),
    state_(AssistState::INVALID) {}

AssistState ParkAssistController::classify(int d) const {
  if (d < dangerCm_)  return AssistState::DANGER;
  if (d < cautionCm_) return AssistState::CAUTION;
  return AssistState::SAFE;
}

void ParkAssistController::update(int distanceCm, bool valid) {
  if (!valid || distanceCm < minValidCm_ || distanceCm > maxValidCm_) {
    state_ = AssistState::INVALID;
    return;
  }

  // Hysteresis: require distance to move beyond thresholds +/- hys_ to change states
  switch (state_) {
    case AssistState::INVALID:
      state_ = classify(distanceCm);
      return;

    case AssistState::DANGER:
      // Stay DANGER until we're clearly above danger threshold
      if (distanceCm >= dangerCm_ + hys_) state_ = classify(distanceCm);
      return;

    case AssistState::CAUTION:
      // Move to DANGER only if clearly below danger threshold
      if (distanceCm < dangerCm_ - hys_) state_ = AssistState::DANGER;
      // Move to SAFE only if clearly above caution threshold
      else if (distanceCm >= cautionCm_ + hys_) state_ = AssistState::SAFE;
      return;

    case AssistState::SAFE:
      // Drop to CAUTION only if clearly below caution threshold
      if (distanceCm < cautionCm_ - hys_) state_ = classify(distanceCm);
      return;
  }
}