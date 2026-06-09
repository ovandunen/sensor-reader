#include "TelemetryJson.h"

#if __has_include(<cmath>)
#include <cmath>
#endif

void TelemetryJson::beginObject() { out_.print('{'); }

void TelemetryJson::endObjectLn() { out_.println('}'); }

void TelemetryJson::kvString(const char* key, const char* value, bool trailingComma) {
  out_.print('\"');
  out_.print(key);
  out_.print("\":\"");
  out_.print(value);
  out_.print('\"');
  if (trailingComma) out_.print(',');
}

void TelemetryJson::kvUInt(const char* key, uint64_t value, bool trailingComma) {
  out_.print('\"');
  out_.print(key);
  out_.print("\":");
  out_.print(value);
  if (trailingComma) out_.print(',');
}

void TelemetryJson::kvFloatOrNull(const char* key,
                                  float value,
                                  uint8_t decimals,
                                  bool trailingComma) {
  out_.print('\"');
  out_.print(key);
  out_.print("\":");

#if __has_include(<Arduino.h>)
  if (isnan(value)) {
    out_.print("null");
  } else {
    out_.print(value, decimals);
  }
#else
  if (std::isnan(static_cast<double>(value))) {
    out_.print("null");
  } else {
    out_.print(value, decimals);
  }
#endif

  if (trailingComma) out_.print(',');
}

