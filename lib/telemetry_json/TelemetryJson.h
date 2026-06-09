#pragma once

/**
 * TelemetryJson
 * Lightweight JSON-lines emitter for Arduino `Print` streams.
 *
 * Design goals:
 * - Small, dependency-free (no ArduinoJson).
 * - Deterministic output (easy to test).
 * - Handles `NaN` floats by emitting JSON `null`.
 */

#if __has_include(<Arduino.h>)
#include <Arduino.h>
#else
// Host-side unit tests: provide a minimal Arduino-Print compatible API.
#include <cstdint>
class Print {
public:
  virtual ~Print() = default;
  virtual size_t print(const char* s) = 0;
  virtual size_t print(char c) = 0;
  virtual size_t print(uint64_t v) = 0;
  virtual size_t print(unsigned long v) = 0;
  virtual size_t print(float v, int decimals) = 0;
  virtual size_t println(char c) = 0;
};
#endif

class TelemetryJson {
public:
  explicit TelemetryJson(Print& out) : out_(out) {}

  void beginObject();
  void endObjectLn();

  void kvString(const char* key, const char* value, bool trailingComma);
  void kvUInt(const char* key, uint64_t value, bool trailingComma);
  void kvFloatOrNull(const char* key, float value, uint8_t decimals, bool trailingComma);

private:
  Print& out_;
};

