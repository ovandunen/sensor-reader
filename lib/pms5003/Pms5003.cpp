#include "Pms5003.h"

bool Pms5003::begin(HardwareSerial& uart, int rxPin, int txPin, uint32_t baud) {
  uart_ = &uart;
  uart_->begin(baud, SERIAL_8N1, rxPin, txPin);
  uart_->setTimeout(50);
  while (uart_->available() > 0) {
    (void)uart_->read();
  }
  return true;
}

bool Pms5003::read(Reading* out, uint32_t timeoutMs) {
  if (!out || !uart_) {
    return false;
  }
  *out = Reading{};

  while (uart_->available() > 0) {
    (void)uart_->read();
  }

  const uint32_t deadline = millis() + timeoutMs;
  uint8_t buf[32];
  size_t n = 0;

  while (millis() < deadline) {
    if (!uart_->available()) {
      delay(1);
      continue;
    }
    const int c = uart_->read();
    if (c < 0) {
      continue;
    }

    if (n == 0 && c != 0x42) {
      continue;
    }
    if (n == 1 && c != 0x4D) {
      n = (c == 0x42) ? 1 : 0;
      continue;
    }

    buf[n++] = static_cast<uint8_t>(c);
    if (n != sizeof(buf)) {
      continue;
    }

    uint16_t sum = 0;
    for (size_t i = 0; i < 30; i++) {
      sum += buf[i];
    }
    const uint16_t chk = static_cast<uint16_t>(buf[30] << 8) | buf[31];
    if (sum != chk) {
      n = 0;
      continue;
    }

    const uint16_t pm1 = static_cast<uint16_t>((buf[10] << 8) | buf[11]);
    const uint16_t pm25 = static_cast<uint16_t>((buf[12] << 8) | buf[13]);
    const uint16_t pm10 = static_cast<uint16_t>((buf[14] << 8) | buf[15]);
    out->pm1_0_ug_m3 = static_cast<float>(pm1);
    out->pm2_5_ug_m3 = static_cast<float>(pm25);
    out->pm10_ug_m3 = static_cast<float>(pm10);
    out->ok = true;
    return true;
  }
  return false;
}
