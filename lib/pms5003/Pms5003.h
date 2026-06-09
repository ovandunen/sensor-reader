#pragma once

#include <Arduino.h>
#include <HardwareSerial.h>

/**
 * Plantower PMS5003 active-mode UART reader (32-byte standard frame).
 * Atmospheric PM concentrations are reported in µg/m³.
 */
class Pms5003 {
 public:
  struct Reading {
    bool ok = false;
    float pm1_0_ug_m3 = NAN;
    float pm2_5_ug_m3 = NAN;
    float pm10_ug_m3 = NAN;
  };

  /** @param uart  e.g. Serial2; must be configured before read(). */
  bool begin(HardwareSerial& uart, int rxPin, int txPin, uint32_t baud = 9600);

  /** Discards stale bytes, then waits up to timeoutMs for one valid frame. */
  bool read(Reading* out, uint32_t timeoutMs = 800);

 private:
  HardwareSerial* uart_ = nullptr;
};
