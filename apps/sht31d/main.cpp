/**
 * SHT31-D smoke-test app — temperature and humidity on USB Serial @ 115200.
 */

#include <Arduino.h>

#include <Sht31dSensor.h>

#include "../../lib/cdc_serial/CdcSerialBegin.h"

static const uint32_t SERIAL_BAUD = 115200;
static const uint32_t READ_INTERVAL_MS = 2000;

static Sht31dSensor sht31;
static uint32_t lastReadMs = 0;

void setup() {
  cdcSerialBegin(SERIAL_BAUD);
  delay(250);
  sht31.begin();
}

void loop() {
  const uint32_t now = millis();
  if ((uint32_t)(now - lastReadMs) >= READ_INTERVAL_MS) {
    lastReadMs = now;
    if (sht31.read()) {
      Serial.print("SHT31-D temperature_c=");
      Serial.print(sht31.temperatureC(), 2);
      Serial.print(" humidity=");
      Serial.println(sht31.humidity(), 2);
    }
  }
}
