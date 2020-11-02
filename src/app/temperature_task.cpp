#include "temperature_task.hpp"

#include <quface-io/engine.hpp>

#include "config.hpp"

using namespace suanzi;
using namespace suanzi::io;

TemperatureTask* TemperatureTask::get_instance() {
  static TemperatureTask instance(
      (io::TemperatureManufacturer)Config::get_temperature().manufacturer);
  return &instance;
}

bool TemperatureTask::idle() { return !get_instance()->is_running_; }

TemperatureTask::TemperatureTask(TemperatureManufacturer m, QThread* thread,
                                 QObject* parent)
    : is_running_(false) {
  auto engine = Engine::instance();
  temperature_reader_ = engine->get_temperature_reader(m);
  if (temperature_reader_ == nullptr) {
    SZ_LOG_ERROR("Get temperature reader error");
    return;
  }

  if (thread == nullptr) {
    static QThread new_thread;
    moveToThread(&new_thread);
    new_thread.start();
  } else {
    moveToThread(thread);
    thread->start();
  }
}

TemperatureTask::~TemperatureTask() {}

void TemperatureTask::rx_update(DetectionRatio detection, bool to_clear) {
  is_running_ = true;

  static TemperatureMatrix mat;
  while (SZ_RETCODE_OK != temperature_reader_->read(mat)) {
    QThread::msleep(100);
    // SZ_LOG_WARN("temperature_reader_->read failed");
  }

  if (!to_clear) {
    detection.x += 0.125f;
    detection.width = std::min(detection.width, 1.f - detection.x);
    detection.height = std::min(detection.height * 1.25f, 1.f - detection.y);
  } else {
    detection.x = 0.45;
    detection.y = 0.45;
    detection.width = 0.1;
    detection.height = 0.1;
  }

  float max_x, max_y;
  float max_temperature = 0;
  for (size_t i = 0; i < 256; i++) {
    float x = (i % 16) / 16.f, y = (i / 16) / 16.f;
    if (detection.x <= x && x <= detection.x + detection.width &&
        detection.y <= y && y <= detection.y + detection.height &&
        mat.value[i] > max_temperature) {
      max_temperature = mat.value[i];
      max_x = x;
      max_y = y;
    }
  }

  if (to_clear)
    max_x = max_y = 0.5;

  emit tx_heatmap(mat, detection, max_x, max_y);

  SZ_LOG_INFO("max temperature={:.2f}", max_temperature);
  is_running_ = false;
}
