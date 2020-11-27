#include "temperature_task.hpp"

#include <quface-io/engine.hpp>

#include "config.hpp"

using namespace suanzi;
using namespace suanzi::io;

static float surface_to_inner(float temperature) {
  if (temperature < 33) return temperature + 2.8;
  if (temperature <= 33.1) return 35.8;
  if (temperature <= 33.2) return 35.9;
  if (temperature <= 33.5) return 36;
  if (temperature <= 33.7) return 36.1;
  if (temperature <= 34.1) return 36.2;
  if (temperature <= 34.3) return 36.3;
  if (temperature <= 34.4) return 36.4;
  if (temperature <= 34.7) return 36.5;
  if (temperature <= 34.8) return 36.6;
  if (temperature <= 35.1) return 36.7;
  if (temperature <= 35.5) return 36.8;
  if (temperature <= 35.7) return 36.9;
  if (temperature <= 36.1) return 37;
  if (temperature <= 36.3) return 37.1;
  if (temperature <= 36.4) return 37.2;
  if (temperature <= 36.5) return 37.3;
  if (temperature <= 36.6) return 37.5;
  if (temperature <= 36.8)
    return 37.6;
  else
    return temperature + 0.8;
}

static float measure_to_surface(float temperature) {
  if (temperature <= 29.6) return temperature + 5.3;
  if (temperature <= 29.7) return 35.1;
  if (temperature <= 29.8) return 35.2;
  if (temperature <= 30.3) return 35.5;
  if (temperature <= 30.4) return 35.6;
  if (temperature <= 30.9) return 35.7;
  if (temperature <= 31.2) return 35.8;
  if (temperature <= 31.3) return 36.1;
  if (temperature <= 31.8) return temperature + 4.8;
  if (temperature <= 31.9) return 36.7;
  if (temperature <= 32)
    return 36.9;
  else
    return temperature + 4.9;
}

TemperatureTask* TemperatureTask::get_instance() {
  static TemperatureTask instance(
      (io::TemperatureManufacturer)Config::get_temperature().manufacturer);
  return &instance;
}

bool TemperatureTask::idle() { return !get_instance()->is_running_; }

TemperatureTask::TemperatureTask(TemperatureManufacturer m, QThread* thread,
                                 QObject* parent)
    : m_(m),
      temperature_reader_(nullptr),
      is_running_(false),
      ambient_temperature_(0),
      face_temperature_(0) {
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

void TemperatureTask::connect() {
  static TemperatureMatrix mat;
  int success = 0;
  do {
    while (temperature_reader_ == nullptr)
      temperature_reader_ = Engine::instance()->get_temperature_reader(m_);
    QThread::msleep(500);

    if (try_reading(mat)) {
      success++;
      emit tx_heatmap_init(success);
    } else {
      temperature_reader_ = nullptr;
      SZ_LOG_WARN("re-connecting for successive failure");
    }

  } while (success < 10);
}

bool TemperatureTask::try_reading(TemperatureMatrix& mat) {
  const int MAX_TRIAL = 10;
  const int INTERVAL = 200;
  int trial = 0;
  while (SZ_RETCODE_OK != temperature_reader_->read(mat) && ++trial < MAX_TRIAL)
    QThread::msleep(INTERVAL);

  bool ret = trial < MAX_TRIAL;
  if (ret) QThread::msleep(INTERVAL);
  return ret;
}

void TemperatureTask::rx_update(DetectionRatio detection, bool to_clear) {
  is_running_ = true;

  if (ambient_temperature_ == 0) connect();

  static TemperatureMatrix mat;
  while (!try_reading(mat)) QThread::msleep(1);

  if (!to_clear) {
    detection.x += 0.125f;
    detection.y -= detection.height * 0.2f;
    detection.width = std::min(detection.width, 1.f - detection.x);
    detection.height = detection.height * 0.4f;
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
    if (pow(x - 0.5f, 2) + pow(y - 0.5f, 2) <= pow(6.f / 16.f, 2) &&
        detection.x <= x && x <= detection.x + detection.width &&
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
    else
      ambient_temperature_ = ambient_temperature_ * 0.9 + max_temperature * 0.1;
    face_temperature_ = 0;
  } else {
    face_temperature_ = max_temperature;
    // SZ_LOG_INFO("ambient={:.2f}°C, face={:.2f}°C", ambient_temperature_,
    //             face_temperature_);

    face_temperature_ = surface_to_inner(
        measure_to_surface(face_temperature_ + Config::get_temperature_bias()));

    emit tx_temperature(face_temperature_);
  }

  emit tx_heatmap(mat, detection, max_x, max_y);

  is_running_ = false;
}
