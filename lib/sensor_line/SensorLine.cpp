#include "SensorLine.h"

#include <cstdio>

void formatSensorsLine(char* buf, size_t cap, const SensorsLine& line) {
  const bool sensorError = line.ds18b20_error || line.sht31_error || line.pms5003_error;
  const char* closing = sensorError ? ",\"sensor_error\":true}" : "}";

  snprintf(buf, cap,
           "{\"type\":\"sensors\",\"node_id\":\"%s\",\"uptime_ms\":%lu,"
           "\"temperature_c\":%.1f,\"humidity\":%.1f,\"pm25\":%u,\"pm10\":%u,"
           "\"ts_ms\":%lu,\"ds18b20_error\":%s,\"sht31_error\":%s,"
           "\"pms5003_error\":%s%s",
           line.node_id, static_cast<unsigned long>(line.uptime_ms), line.temperature_c,
           line.humidity, static_cast<unsigned>(line.pm25), static_cast<unsigned>(line.pm10),
           static_cast<unsigned long>(line.ts_ms), line.ds18b20_error ? "true" : "false",
           line.sht31_error ? "true" : "false", line.pms5003_error ? "true" : "false", closing);
}
