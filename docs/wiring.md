# Wiring — SunFounder Cam Ext. / unified sensor hub

Pin assignments match **`esp32_vollstaendiges_verdrahtungsdiagramm.svg`** and the constants in
`apps/sensor_hub/main.cpp`. Production firmware is **`sensor_hub`** — one ESP32, all three
sensors on one board, fused JSON on USB `Serial` @ **115200**.

## Pin map

| Function | Sensor | ESP32 GPIO | Firmware |
|----------|--------|------------|----------|
| Temperature | DS18B20 (OneWire) | **13** DATA + 4.7 kΩ → 3.3 V | `Ds18b20Sensor(13)` |
| Particulate | PMS5003 | **19** RX ← sensor TX, **18** TX → sensor RX | `Serial1` @ 9600 |
| Humidity / temp | SHT31-D (I²C) | **21** SDA, **22** SCL | `Wire`, address **0x44** |
| USB telemetry | Host (Serial Monitor, BMS, …) | USB CDC / UART bridge | `Serial` @ 115200 |

**Do not use GPIO4** for DS18B20 on this board revision (flash strapping).

**GPIO16 / GPIO17** are UART2 on this diagram — not assigned by `sensor-reader` firmware.
Leave them unconnected for sensors in this project.

Some board revisions route UART2 to GPIO32/33 instead of 16/17 — check silkscreen before
using UART2 for anything else.

---

## DS18B20

```
DS18B20 DATA ---- GPIO13 ---- 4.7 kΩ ---- 3.3 V
DS18B20 VCC  ---- 3.3 V
DS18B20 GND  ---- GND (common)
```

- OneWire bus; only the hub / `temp_ds18b20` app reads this sensor.
- Default pin is **GPIO13** (`lib/ds18b20/Ds18b20Sensor.h`).

---

## PMS5003

```
PMS5003 TX (pin 5) -----> GPIO19 (ESP32 RX, Serial1)
PMS5003 RX (pin 4) <----- GPIO18 (ESP32 TX, optional in active mode)
PMS5003 VCC ---------- 5 V
PMS5003 GND ---------- GND (common)
PMS5003 SET ---------- 3.3 V (continuous active sampling)
```

- **UART1 (`Serial1`)** at **9600** baud — keeps USB `Serial` (UART0) free for JSON output.
- TX/RX are **crossed**: sensor TX → ESP32 RX, sensor RX → ESP32 TX.
- Hub maps readings to JSON fields **`pm25`** and **`pm10`**.

---

## SHT31-D

```
SHT31-D SDA ---- GPIO21 ---- 4.7 kΩ ---- 3.3 V
SHT31-D SCL ---- GPIO22 ---- 4.7 kΩ ---- 3.3 V
SHT31-D VCC ---- 3.3 V
SHT31-D GND ---- GND (common)
```

- I²C address **0x44** (configured in `lib/sht31d/Sht31dSensor.cpp`).
- **Two** external 4.7 kΩ pull-ups (SDA and SCL to 3.3 V) are required on the bus.
- Hub maps **`humidity`** (% RH) from this sensor; **`temperature_c`** in the fused line
  comes from **DS18B20**, not SHT31-D.

---

## USB serial (telemetry)

- All apps emit newline-delimited JSON on **`Serial` @ 115200**.
- Open the board’s USB serial port at the same baud in the Arduino Serial Monitor or your
  host parser (`BmsDataParser.kt` expects hub keys: `temperature_c`, `humidity`, `pm25`,
  `pm10`, `ts_ms`, …).

---

## Build and flash

From the `sensor-reader/` repo root:

```bash
pio run -e sensor_hub              # production — all sensors
pio run -t upload -e sensor_hub
pio device monitor -b 115200

pio run -e temp_ds18b20            # DS18B20 smoke test only
pio run -e pms5003                 # PMS5003 smoke test only
pio run -e sht31d                  # SHT31-D smoke test only
```
