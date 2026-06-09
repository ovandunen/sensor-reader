#include "Sht31dSensor.h"

#include <Wire.h>

static const uint8_t kSht31Address = 0x44;
static const int kSdaPin = 21;
static const int kSclPin = 22;

Sht31dSensor::Sht31dSensor()
    : initialized_(false), lastTempC_(NAN), lastHumidity_(NAN) {}

void Sht31dSensor::begin() {
  Wire.begin(kSdaPin, kSclPin);
  initialized_ = sht31_.begin(kSht31Address);
}

bool Sht31dSensor::read() {
  if (!initialized_) {
    return false;
  }

  const float tempC = sht31_.readTemperature();
  const float humidity = sht31_.readHumidity();
  if (isnan(tempC) || isnan(humidity)) {
    return false;
  }

  lastTempC_ = tempC;
  lastHumidity_ = humidity;
  return true;
}

float Sht31dSensor::temperatureC() const { return lastTempC_; }

float Sht31dSensor::humidity() const { return lastHumidity_; }
