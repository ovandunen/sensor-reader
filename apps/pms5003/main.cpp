/**
 * PMS5003 particulate sensor — newline-delimited JSON on USB Serial @ 115200.
 *
 * Wiring (UART1):
 * - PMS5003 TX -> ESP32 GPIO19 (UART1 RX)
 * - PMS5003 RX -> ESP32 GPIO18 (UART1 TX, optional in active mode)
 * - PMS5003 VCC 5V, GND common; SET high for active sampling
 */

#include <HardwareSerial.h>

#include <TelemetryJson.h>

#include "../../lib/cdc_serial/CdcSerialBegin.h"
#include "../../lib/pms5003/Pms5003.h"

static const char* APP_NAME = "dust_sensor";
static const char* NODE_ID = "esp32-pms-01";

static const uint32_t SERIAL_BAUD = 115200;
static const uint32_t READ_INTERVAL_MS = 2000;

static const int kPmsRxPin = 19;  // ESP RX <= PMS TX
static const int kPmsTxPin = 18;  // ESP TX => PMS RX
static const uint32_t kPmsBaud = 9600;

static HardwareSerial SerialPms(1);
static Pms5003 pms;
static TelemetryJson telemetry(Serial);

static uint32_t readErrors = 0;
static uint32_t lastReadMs = 0;

static Pms5003::Reading lastReading{};

static void emitStatus(bool ok, const char* message = nullptr) {
  const uint32_t t = millis();
  telemetry.beginObject();
  telemetry.kvString("app", APP_NAME, true);
  telemetry.kvString("type", "status", true);
  telemetry.kvString("node_id", NODE_ID, true);
  telemetry.kvUInt("uptime_ms", t, true);
  telemetry.kvUInt("timestamp_ms", t, true);
  telemetry.kvString("sensor", "pms5003", true);
  if (ok) {
    telemetry.kvString("status", "started", false);
  } else {
    telemetry.kvString("status", "error", true);
    telemetry.kvString("message", message ? message : "init failed", false);
  }
  telemetry.endObjectLn();
}

static void readParticulateOnce() {
  Pms5003::Reading r{};
  if (!pms.read(&r)) {
    readErrors++;
    lastReading = Pms5003::Reading{};
    return;
  }
  lastReading = r;
}

static void emitParticulate() {
  const uint32_t uptime = millis();
  telemetry.beginObject();
  telemetry.kvString("app", APP_NAME, true);
  telemetry.kvString("type", "particulate", true);
  telemetry.kvString("node_id", NODE_ID, true);
  telemetry.kvUInt("uptime_ms", uptime, true);
  telemetry.kvUInt("timestamp_ms", uptime, true);

  const float pm1 = lastReading.ok ? lastReading.pm1_0_ug_m3 : NAN;
  const float pm25 = lastReading.ok ? lastReading.pm2_5_ug_m3 : NAN;
  const float pm10 = lastReading.ok ? lastReading.pm10_ug_m3 : NAN;

  telemetry.kvFloatOrNull("pm1_0_ug_m3", pm1, 1, true);
  telemetry.kvFloatOrNull("pm2_5_ug_m3", pm25, 1, true);
  telemetry.kvFloatOrNull("pm10_ug_m3", pm10, 1, true);
  telemetry.kvUInt("read_errors", readErrors, false);
  telemetry.endObjectLn();
}

void setup() {
  cdcSerialBegin(SERIAL_BAUD);
  delay(250);

  const bool uartOk = pms.begin(SerialPms, kPmsRxPin, kPmsTxPin, kPmsBaud);
  emitStatus(uartOk, uartOk ? nullptr : "UART1 init failed");
}

void loop() {
  const uint32_t now = millis();
  if ((uint32_t)(now - lastReadMs) >= READ_INTERVAL_MS) {
    lastReadMs = now;
    readParticulateOnce();
    emitParticulate();
  }
}
