#include "temperature_task.hpp"

#include <quface-io/engine.hpp>

using namespace suanzi;
using namespace suanzi::io;

TemperatureTask::TemperatureTask(TemperatureManufacturer m) {
  auto engine = Engine::instance();
  temperature_reader_ = engine->get_temperature_reader(m);
  if (temperature_reader_ == nullptr) {
    SZ_LOG_ERROR("Get temperature reader error");
    return;
  }

  start();
}

TemperatureTask::~TemperatureTask() {}

void TemperatureTask::rx_enable() { is_enabled_ = true; }

void TemperatureTask::rx_disable() { is_enabled_ = false; }

void TemperatureTask::run() {
  while (true) {
    if (is_enabled_) {
      float temperature;
      SZ_RETCODE ret = temperature_reader_->read(temperature);
      if (ret == SZ_RETCODE_OK) {
        tx_temperature(temperature);
      }
    }
    usleep(250000);
  }
}
