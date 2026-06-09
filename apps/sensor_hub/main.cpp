/**
 * Sensor hub — single ESP32, fused JSON line per 2 s cycle.
 * DS18B20 + PMS5003 (UART1) + SHT31-D (I²C) on one board.
 */

#include <Arduino.h>
#include <HardwareSerial.h>

#include <Ds18b20Sensor.h>
#include <Pms5003.h>
#include <Sht31dSensor.h>

#include "../../lib/cdc_serial/CdcSerialBegin.h"

static const char *NODE_ID = "esp32-hub-01";
static const uint32_t SERIAL_BAUD = 115200;
// 2 s cycle — matches BmsDataParser staleness window
static const uint32_t READ_INTERVAL_MS = 2000;

static const uint8_t PIN_ONEWIRE = 13;
static const int kPmsRxPin = 19;
static const int kPmsTxPin = 18;
static const uint32_t kPmsBaud = 9600;

// NOTE: GPIO16/17 reserved for TF02-Pro LiDAR (future).
// WARNING: Some SunFounder board revisions cannot use GPIO16/17 freely —
// verify against your specific board revision before wiring LiDAR.
// See docs/upstream_patches.md and wiring diagram for context.

static Ds18b20Sensor ds18b20(PIN_ONEWIRE);
static HardwareSerial SerialPms(1);
static Pms5003 pms;
static Sht31dSensor sht31;

static uint32_t lastCycleMs = 0;

static float lastTemperatureC = 0.0f;
static float lastHumidity = 0.0f;
static uint16_t lastPm25 = 0;
static uint16_t lastPm10 = 0;

static char lineBuf[256];

static void emitSensorsJson(uint32_t uptime, bool sensorError) {
  if (sensorError) {
    snprintf(lineBuf, sizeof(lineBuf),
             "{\"type\":\"sensors\",\"node_id\":\"%s\",\"uptime_ms\":%lu,"
             "\"temperature_c\":%.1f,\"humidity\":%.1f,\"pm25\":%u,\"pm10\":%u,"
             "\"ts_ms\":%lu,\"sensor_error\":true}",
             NODE_ID, static_cast<unsigned long>(uptime), lastTemperatureC, lastHumidity,
             lastPm25, lastPm10, static_cast<unsigned long>(uptime));
  } else {
    snprintf(lineBuf, sizeof(lineBuf),
             "{\"type\":\"sensors\",\"node_id\":\"%s\",\"uptime_ms\":%lu,"
             "\"temperature_c\":%.1f,\"humidity\":%.1f,\"pm25\":%u,\"pm10\":%u,"
             "\"ts_ms\":%lu}",
             NODE_ID, static_cast<unsigned long>(uptime), lastTemperatureC, lastHumidity,
             lastPm25, lastPm10, static_cast<unsigned long>(uptime));
  }
  Serial.println(lineBuf);
}

static void runSensorCycle() {
  bool sensorError = false;

  if (ds18b20.read()) {
    lastTemperatureC = ds18b20.temperatureC();
  } else {
    sensorError = true;
  }

  Pms5003::Reading pmsReading{};
  if (pms.read(&pmsReading) && pmsReading.ok) {
    // Pms5003::Reading stores PM as float; cast to uint16_t for JSON output.
    lastPm25 = static_cast<uint16_t>(pmsReading.pm2_5_ug_m3);
    lastPm10 = static_cast<uint16_t>(pmsReading.pm10_ug_m3);
  } else {
    sensorError = true;
  }

  if (sht31.read()) {
    lastHumidity = sht31.humidity();
  } else {
    sensorError = true;
  }

  const uint32_t uptime = millis();
  emitSensorsJson(uptime, sensorError);
}

void setup() {
  cdcSerialBegin(SERIAL_BAUD);
  delay(250);

  ds18b20.begin();
  pms.begin(SerialPms, kPmsRxPin, kPmsTxPin, kPmsBaud);
  sht31.begin();
}

void loop() {
  const uint32_t now = millis();
  if ((uint32_t)(now - lastCycleMs) >= READ_INTERVAL_MS) {
    lastCycleMs = now;
    runSensorCycle();
  }
}
