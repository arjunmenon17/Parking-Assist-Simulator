#include "Ultrasonic.h"
#include "MedianFilter.h"
#include "Controller.h"
#include "Outputs.h"

// ==== Pin assignments ====
static constexpr uint8_t PIN_TRIG = 9;
static constexpr uint8_t PIN_ECHO = 10;

// LEDs (you wired these)
static constexpr uint8_t PIN_LED_GREEN  = 3;
static constexpr uint8_t PIN_LED_YELLOW = 5;
static constexpr uint8_t PIN_LED_RED    = 6;

// ==== Timing ====
static constexpr uint32_t SENSOR_PERIOD_MS = 50;   // 20 Hz sampling
static constexpr uint32_t LOG_PERIOD_MS    = 200;  // serial logging rate

// ==== Components ====
UltrasonicSensor sensor(PIN_TRIG, PIN_ECHO);

// Median-of-5 filter for distance in cm
MedianFilter5<int> distFilter;

// Controller thresholds (cm) + hysteresis (cm)
ParkAssistController controller(
  /*danger_cm=*/30,
  /*caution_cm=*/60,
  /*min_valid_cm=*/2,
  /*max_valid_cm=*/400,
  /*hysteresis_cm=*/3
);

LedOutputs leds(PIN_LED_GREEN, PIN_LED_YELLOW, PIN_LED_RED);

// ==== Scheduling ====
uint32_t lastSensorMs = 0;
uint32_t lastLogMs = 0;

void setup() {
  Serial.begin(115200);

  sensor.begin();
  leds.begin();

  Serial.println("Parking Assist Simulator: Milestone 2 (LED zones + filtering + FSM)");
}

void loop() {
  const uint32_t now = millis();

  // Sample sensor on a fixed schedule (non-blocking)
  if (now - lastSensorMs >= SENSOR_PERIOD_MS) {
    lastSensorMs = now;

    DistanceReading r = sensor.readDistanceCm();
    if (r.valid) {
      int filtered = distFilter.push((int)r.cm);
      controller.update(filtered, true);
    } else {
      controller.update(0, false);
    }

    leds.apply(controller.state());
  }

  // Periodic logging
  if (now - lastLogMs >= LOG_PERIOD_MS) {
    lastLogMs = now;

    Serial.print("state=");
    Serial.print(toString(controller.state()));
    Serial.print("  filtered_cm=");
    Serial.println(distFilter.currentOr(-1));
  }
}