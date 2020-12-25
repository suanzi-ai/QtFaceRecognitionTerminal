#include "single_temperature_app.hpp"
#include <cmath>
#include <quface-io/engine.hpp>
#include "config.hpp"
#include "temperature_task.hpp"

using namespace suanzi;
using namespace suanzi::io;

SingleTemperatureApp::SingleTemperatureApp(TemperatureTask *temperature_task)
    : TemperatureApp(temperature_task) {
  init();
}

void SingleTemperatureApp::read_temperature(const QRectF &face_area,
                                            bool valid_face_area) {
  if (temperature_reader_ == nullptr) {
    init();
    return;
  }

  float face_temperature = 0.0;
  if (SZ_RETCODE_OK == temperature_reader_->read(face_temperature)) {
    emit temperature_task_->tx_temperature(face_temperature);
  }
}
