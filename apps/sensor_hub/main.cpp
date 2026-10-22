/**
 * Sensor hub — single ESP32, fused JSON line per 2 s cycle.
 * DS18B20 + PMS5003 (UART1) + SHT31-D (I²C) on one board.
 */

#include <Arduino.h>
#include <HardwareSerial.h>

#include <Ds18b20Sensor.h>
#include <Pms5003.h>
#include <SensorLine.h>
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

static void runSensorCycle() {
  bool ds18b20Error = false;
  bool sht31Error = false;
  bool pms5003Error = false;

  if (ds18b20.read()) {
    lastTemperatureC = ds18b20.temperatureC();
  } else {
    ds18b20Error = true;
  }

  Pms5003::Reading pmsReading{};
  if (pms.read(&pmsReading) && pmsReading.ok) {
    // Pms5003::Reading stores PM as float; cast to uint16_t for JSON output.
    lastPm25 = static_cast<uint16_t>(pmsReading.pm2_5_ug_m3);
    lastPm10 = static_cast<uint16_t>(pmsReading.pm10_ug_m3);
  } else {
    pms5003Error = true;
  }

  if (sht31.read()) {
    lastHumidity = sht31.humidity();
  } else {
    sht31Error = true;
  }

  const uint32_t uptime = millis();
  SensorsLine line;
  line.node_id = NODE_ID;
  line.uptime_ms = uptime;
  line.temperature_c = lastTemperatureC;
  line.humidity = lastHumidity;
  line.pm25 = lastPm25;
  line.pm10 = lastPm10;
  line.ts_ms = uptime;
  line.ds18b20_error = ds18b20Error;
  line.sht31_error = sht31Error;
  line.pms5003_error = pms5003Error;
  formatSensorsLine(lineBuf, sizeof(lineBuf), line);
  Serial.println(lineBuf);
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
