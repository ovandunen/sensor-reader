#pragma once

/**
 * USB-serial startup helper for CDC-friendly telemetry.
 *
 * - On boards with USB CDC routed to Serial (Arduino: "USB CDC On Boot: Enabled"),
 *   waits briefly until a USB host attaches so the first line is less likely to be lost.
 * - On UART + external bridge (classic ESP32 + CP2102/CH340), Serial is ready quickly;
 *   the macro block is skipped so setup() does not hang.
 *
 * Open the host serial port at the same baud (115200) as Serial.begin here.
 */
#include <Arduino.h>

inline void cdcSerialBegin(uint32_t baud = 115200) {
  Serial.begin(baud);
#if defined(ARDUINO_USB_CDC_ON_BOOT) && ARDUINO_USB_CDC_ON_BOOT
  constexpr uint32_t kMaxWaitMs = 5000;
  const uint32_t t0 = millis();
  while (!Serial && (millis() - t0) < kMaxWaitMs) {
    delay(10);
  }
#endif
}
