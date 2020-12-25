#include "temperature_app.hpp"
#include <quface-io/engine.hpp>
#include <quface-io/temperature.hpp>

#include "config.hpp"

using namespace suanzi;
using namespace suanzi::io;

TemperatureApp::TemperatureApp(TemperatureTask *temperature_task)
    : temperature_task_(temperature_task) {
  temperature_reader_ = nullptr;
}

TemperatureApp::~TemperatureApp() {}

bool TemperatureApp::init() {
  if (temperature_reader_ == nullptr) {
    TemperatureManufacturer temp_sensor_type =
        (TemperatureManufacturer)Config::get_temperature().manufacturer;
    temperature_reader_ =
        Engine::instance()->get_temperature_reader(temp_sensor_type);
  }
  return temperature_reader_ != nullptr;
}
