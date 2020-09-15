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

void TemperatureTask::rx_enable_read_temperature(bool enable_read_temperature) {
  enable_read_temperature_ = enable_read_temperature;
}

void TemperatureTask::run() {
  while (true) {
    if (enable_read_temperature_) {
      float temperature;
      SZ_RETCODE ret = temperature_reader_->read(temperature);
      if (ret == SZ_RETCODE_OK) {
        tx_temperature(temperature);
      }
    }
    usleep(250000);
  }
}
