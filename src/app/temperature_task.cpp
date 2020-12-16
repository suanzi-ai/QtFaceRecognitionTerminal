#include "temperature_task.hpp"
#include <cmath>
#include "matrix_temperature_app.hpp"

#include <quface-io/engine.hpp>

#include "config.hpp"

using namespace suanzi;
using namespace suanzi::io;

TemperatureTask *TemperatureTask::get_instance() {
  static TemperatureTask instance;
  return &instance;
}

bool TemperatureTask::idle() { return !get_instance()->is_running_; }

TemperatureTask::TemperatureTask(QThread *thread, QObject *parent)
    : is_running_(false), QObject(parent) {
  if (thread == nullptr) {
    static QThread new_thread;
    moveToThread(&new_thread);
    new_thread.start();
  } else {
    moveToThread(thread);
    thread->start();
  }

  temperature_app_ = new MatrixTemperatureApp(this);
}

TemperatureTask::~TemperatureTask() { delete temperature_app_; }

void TemperatureTask::rx_update(DetectionRatio detection, bool valid_face) {
  is_running_ = true;

  temperature_app_->read_temperature(
      QRectF(detection.x, detection.y, detection.width, detection.height),
      valid_face);

  is_running_ = false;
}
