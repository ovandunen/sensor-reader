#pragma once

#include <Arduino.h>

class Ds18b20Sensor {
 public:
  explicit Ds18b20Sensor(uint8_t pin = 13);

  void begin();
  bool read();
  float temperatureC() const;

 private:
  uint8_t pin_;
  bool sensorFound_;
  float lastTempC_;
  void* oneWire_;
  void* dallas_;
  uint8_t address_[8];
};
