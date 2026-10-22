#pragma once

/**
 * One sensor-hub JSON object, without a trailing newline.
 * The three per-sensor keys are always present.
 * "sensor_error" is present only when any of those three is true.
 */

#include <cstddef>
#include <cstdint>

struct SensorsLine {
  const char* node_id;
  uint32_t uptime_ms;
  float temperature_c;
  float humidity;
  uint16_t pm25;
  uint16_t pm10;
  uint32_t ts_ms;
  bool ds18b20_error;
  bool sht31_error;
  bool pms5003_error;
};

void formatSensorsLine(char* buf, size_t cap, const SensorsLine& line);
