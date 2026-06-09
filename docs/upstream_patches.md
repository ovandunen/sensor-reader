# Upstream library patches

This document records every manual change made to vendored libraries under
`libraries/`. These libraries are not managed by platformio lib_deps — they are
copied snapshots. Any upstream update must be reconciled against these patches.

---

## OneWire — OneWire.cpp

| Field      | Value |
|------------|-------|
| File       | `libraries/OneWire/OneWire.cpp` |
| Upstream   | https://github.com/PaulStoffregen/OneWire |
| Applied    | 2026-06-08 |
| Reason     | Invalid `#undef` block caused compile error under ESP32 Arduino framework |
| Marker     | `PATCH START` / `PATCH END` comments at patch site |

### What was changed

The invalid `#undef` block was removed and replaced with a short comment. Exact diff:

```diff
-// undef defines for no particular reason
-#ifdef ARDUINO_ARCH_ESP32
-#  undef noInterrupts() {portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;portENTER_CRITICAL(&mux)
-#  undef interrupts() portEXIT_CRITICAL(&mux);}
-#endif
+// ── PATCH START ──────────────────────────────────────────────────────────────
+// Reason:  Invalid #undef block caused compile error on ESP32 Arduino framework.
+// Applied: sensor-reader scaffolding, 2026-06-08.
+// Upstream: https://github.com/PaulStoffregen/OneWire
+// To verify: if updating OneWire from upstream, re-check this block compiles
+//            cleanly under ESP32 Arduino before removing these markers.
+// ESP32 noInterrupts/interrupts macros left in place for this build.
+// ── PATCH END ────────────────────────────────────────────────────────────────
```

### How to re-apply if updating OneWire

1. Copy new upstream `OneWire.cpp` into `libraries/OneWire/`.
2. Run `pio run -e sensor_hub`.
3. If compile error recurs at the same location, re-apply this patch and update
   the date in this document and in the in-file comment.
4. If the upstream fix resolves the issue, remove the patch markers and delete
   this entry.
