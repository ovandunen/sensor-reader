#include <unity.h>

#include <SensorLine.h>

static const char* kAllOk =
    "{\"type\":\"sensors\",\"node_id\":\"esp32-hub-01\",\"uptime_ms\":1000,"
    "\"temperature_c\":21.5,\"humidity\":40.0,\"pm25\":12,\"pm10\":20,"
    "\"ts_ms\":1000,\"ds18b20_error\":false,\"sht31_error\":false,\"pms5003_error\":false}";

static SensorsLine sample(bool ds18b20, bool sht31, bool pms5003) {
  SensorsLine line;
  line.node_id = "esp32-hub-01";
  line.uptime_ms = 1000;
  line.temperature_c = 21.5f;
  line.humidity = 40.0f;
  line.pm25 = 12;
  line.pm10 = 20;
  line.ts_ms = 1000;
  line.ds18b20_error = ds18b20;
  line.sht31_error = sht31;
  line.pms5003_error = pms5003;
  return line;
}

static void expectLine(bool ds18b20, bool sht31, bool pms5003, const char* expected) {
  char buf[256];
  formatSensorsLine(buf, sizeof(buf), sample(ds18b20, sht31, pms5003));
  TEST_ASSERT_EQUAL_STRING(expected, buf);
}

void test_all_sensors_ok() { expectLine(false, false, false, kAllOk); }

void test_ds18b20_failed() {
  expectLine(true, false, false,
             "{\"type\":\"sensors\",\"node_id\":\"esp32-hub-01\",\"uptime_ms\":1000,"
             "\"temperature_c\":21.5,\"humidity\":40.0,\"pm25\":12,\"pm10\":20,"
             "\"ts_ms\":1000,\"ds18b20_error\":true,\"sht31_error\":false,"
             "\"pms5003_error\":false,\"sensor_error\":true}");
}

void test_sht31_failed() {
  expectLine(false, true, false,
             "{\"type\":\"sensors\",\"node_id\":\"esp32-hub-01\",\"uptime_ms\":1000,"
             "\"temperature_c\":21.5,\"humidity\":40.0,\"pm25\":12,\"pm10\":20,"
             "\"ts_ms\":1000,\"ds18b20_error\":false,\"sht31_error\":true,"
             "\"pms5003_error\":false,\"sensor_error\":true}");
}

void test_pms5003_failed() {
  expectLine(false, false, true,
             "{\"type\":\"sensors\",\"node_id\":\"esp32-hub-01\",\"uptime_ms\":1000,"
             "\"temperature_c\":21.5,\"humidity\":40.0,\"pm25\":12,\"pm10\":20,"
             "\"ts_ms\":1000,\"ds18b20_error\":false,\"sht31_error\":false,"
             "\"pms5003_error\":true,\"sensor_error\":true}");
}

void test_two_failed() {
  expectLine(true, true, false,
             "{\"type\":\"sensors\",\"node_id\":\"esp32-hub-01\",\"uptime_ms\":1000,"
             "\"temperature_c\":21.5,\"humidity\":40.0,\"pm25\":12,\"pm10\":20,"
             "\"ts_ms\":1000,\"ds18b20_error\":true,\"sht31_error\":true,"
             "\"pms5003_error\":false,\"sensor_error\":true}");
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_all_sensors_ok);
  RUN_TEST(test_ds18b20_failed);
  RUN_TEST(test_sht31_failed);
  RUN_TEST(test_pms5003_failed);
  RUN_TEST(test_two_failed);
  return UNITY_END();
}
