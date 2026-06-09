#include "Ds18b20Sensor.h"

#include <DallasTemperature.h>
#include <OneWire.h>

Ds18b20Sensor::Ds18b20Sensor(uint8_t pin)
    : pin_(pin), sensorFound_(false), lastTempC_(NAN), oneWire_(nullptr), dallas_(nullptr) {}

void Ds18b20Sensor::begin() {
  if (oneWire_ == nullptr) {
    oneWire_ = new OneWire(pin_);
    dallas_ = new DallasTemperature(static_cast<OneWire*>(oneWire_));
  }

  auto* sensors = static_cast<DallasTemperature*>(dallas_);
  sensors->begin();
  sensorFound_ = (sensors->getDeviceCount() > 0) && sensors->getAddress(address_, 0);
  if (sensorFound_) {
    sensors->setResolution(address_, 12);
  }
}

bool Ds18b20Sensor::read() {
  if (!sensorFound_ || dallas_ == nullptr) {
    return false;
  }

  auto* sensors = static_cast<DallasTemperature*>(dallas_);
  sensors->requestTemperatures();

  const float tempC = sensors->getTempC(address_);
  if (tempC == DEVICE_DISCONNECTED_C) {
    return false;
  }
  if (tempC < -55.0f || tempC > 125.0f) {
    return false;
  }

  lastTempC_ = tempC;
  return true;
}

float Ds18b20Sensor::temperatureC() const { return lastTempC_; }
