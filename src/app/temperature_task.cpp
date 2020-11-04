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
    : m_(m), is_running_(false), ambient_temperature_(0), face_temperature_(0) {
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
  if (to_clear && ambient_temperature_ > 0)
    return;

  is_running_ = true;

  if (ambient_temperature_ == 0)
    QThread::msleep(10000);

  static TemperatureMatrix mat;
  int trial = 0;
  while (SZ_RETCODE_OK != temperature_reader_->read(mat)) {
    QThread::msleep(200);
    trial += 1;

    if (trial >= 20) {
      SZ_LOG_WARN("temperature_reader_->read failed for {} times", trial);
      SZ_LOG_INFO("reconnect temperature_reader_ after 10 seconds");
      trial = 0;
      QThread::msleep(10000);

      SZ_LOG_INFO("reconnect");
    }
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

  if (to_clear) {
    max_x = max_y = 0.5;

    // update ambient temperature if no detection
    if (ambient_temperature_ == 0)
      ambient_temperature_ = max_temperature;
    face_temperature_ = 0;
  } else {
    face_temperature_ = max_temperature;
    // SZ_LOG_INFO("ambient={:.2f}°C, face={:.2f}°C", ambient_temperature_,
    //             face_temperature_);
    emit tx_temperature(face_temperature_);
  }

  emit tx_heatmap(mat, detection, max_x, max_y);

  is_running_ = false;
}
