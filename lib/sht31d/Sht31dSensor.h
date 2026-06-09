#pragma once

#include <Arduino.h>
#include <Adafruit_SHT31.h>

class Sht31dSensor {
 public:
  Sht31dSensor();

  void begin();
  bool read();
  float temperatureC() const;
  float humidity() const;

 private:
  Adafruit_SHT31 sht31_;
  bool initialized_;
  float lastTempC_;
  float lastHumidity_;
};
