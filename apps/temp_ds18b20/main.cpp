/**
 * DS18B20 smoke-test app — newline-delimited JSON on USB Serial @ 115200.
 */

#include <Arduino.h>

#include <Ds18b20Sensor.h>
#include <TelemetryJson.h>

#include "../../lib/cdc_serial/CdcSerialBegin.h"

static const char *APP_NAME = "temp_sensor";
static const char *NODE_ID = "esp32-temp-01";

static const uint32_t SERIAL_BAUD = 115200;
static const uint32_t READ_INTERVAL_MS = 2000;
static const uint8_t PIN_ONEWIRE = 13;

static Ds18b20Sensor ds18b20(PIN_ONEWIRE);
static TelemetryJson telemetry(Serial);

static uint32_t readErrors = 0;
static uint32_t lastReadMs = 0;

static void emitStartupStatus() {
  const uint32_t t = millis();
  telemetry.beginObject();
  telemetry.kvString("app", APP_NAME, true);
  telemetry.kvString("node_id", NODE_ID, true);
  telemetry.kvString("type", "status", true);
  telemetry.kvUInt("uptime_ms", t, true);
  telemetry.kvUInt("timestamp_ms", t, true);
  telemetry.kvString("status", "started", false);
  telemetry.endObjectLn();
}

static void emitReading() {
  const uint32_t uptime = millis();
  telemetry.beginObject();
  telemetry.kvString("app", APP_NAME, true);
  telemetry.kvString("type", "temperature", true);
  telemetry.kvString("node_id", NODE_ID, true);
  telemetry.kvUInt("uptime_ms", uptime, true);
  telemetry.kvUInt("timestamp_ms", uptime, true);

  if (ds18b20.read()) {
    telemetry.kvFloatOrNull("temperature_c", ds18b20.temperatureC(), 2, true);
    telemetry.kvUInt("read_errors", readErrors, false);
  } else {
    readErrors++;
    telemetry.kvString("error", "read_failed", true);
    telemetry.kvUInt("read_errors", readErrors, false);
  }

  telemetry.endObjectLn();
}

void setup() {
  cdcSerialBegin(SERIAL_BAUD);
  delay(250);
  ds18b20.begin();
  emitStartupStatus();
}

void loop() {
  const uint32_t now = millis();
  if ((uint32_t)(now - lastReadMs) >= READ_INTERVAL_MS) {
    lastReadMs = now;
    emitReading();
  }
}
