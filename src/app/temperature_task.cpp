#include "temperature_task.hpp"

#include <quface-io/engine.hpp>

using namespace suanzi;
using namespace suanzi::io;

TemperatureTask::TemperatureTask(TemperatureManufacturer m) {
  auto engine = Engine::instance();
  SZ_RETCODE ret = engine->temperature_start(m);
  if (ret != SZ_RETCODE_OK) {
    SZ_LOG_ERROR("Start temperature error");
  }

  start();
}

TemperatureTask::~TemperatureTask() {
  auto engine = Engine::instance();
  SZ_RETCODE ret = engine->temperature_stop();
  if (ret != SZ_RETCODE_OK) {
    SZ_LOG_ERROR("Stop temperature error");
  }
}

void TemperatureTask::rx_enable_read_temperature(bool enable_read_temperature) {
  enable_read_temperature_ = enable_read_temperature;
}

void TemperatureTask::run() {
  auto engine = Engine::instance();

  while (true) {
    if (enable_read_temperature_) {
      float temperature;
      SZ_RETCODE ret = engine->temperature_read(temperature);
      if (ret == SZ_RETCODE_OK) {
        tx_temperature(temperature);
      }
    }
    usleep(250000);
  }
}
