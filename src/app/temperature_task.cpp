#include "temperature_task.hpp"

#include <cmath>

#include <quface-io/engine.hpp>

#include "config.hpp"

#define VALUE_AT(m, x, y) ((m.value)[(y)*16 + (x)])

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

TemperatureTask *TemperatureTask::get_instance() {
  static TemperatureTask instance(
      (io::TemperatureManufacturer)Config::get_temperature().manufacturer);
  return &instance;
}

bool TemperatureTask::idle() { return !get_instance()->is_running_; }

TemperatureTask::TemperatureTask(TemperatureManufacturer m, QThread *thread,
                                 QObject *parent)
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
  if (temperature_reader_ == nullptr)
    temperature_reader_ = Engine::instance()->get_temperature_reader(m_);

  int success = 0;
  while (temperature_reader_ != nullptr && success < 10) {
    if (try_reading(temperature_matrix_)) {
      success++;
      emit tx_heatmap_init(success);
      QThread::msleep(500);
    } else {
      temperature_reader_ = nullptr;
      SZ_LOG_WARN("re-connecting for successive failure");
    }
  }
}

void TemperatureTask::rotate_temperature_matrix(TemperatureMatrix &mat) {
  auto cfg = Config::get_temperature();
  switch (cfg.sensor_rotation) {
    case TemperatureRotation::ROTATION_90:
      for (int y = 0; y < 16 / 2; y++) {
        for (int x = y; x < 16 - y - 1; x++) {
          float t = VALUE_AT(mat, x, y);
          VALUE_AT(mat, x, y) = VALUE_AT(mat, y, 16 - x - 1);
          VALUE_AT(mat, y, 16 - x - 1) = VALUE_AT(mat, 16 - x - 1, 16 - y - 1);
          VALUE_AT(mat, 16 - x - 1, 16 - y - 1) = VALUE_AT(mat, 16 - y - 1, x);
          VALUE_AT(mat, 16 - y - 1, x) = t;
        }
      }
      break;
    case TemperatureRotation::ROTATION_180:
      for (int y = 0; y < 16 / 2; y++) {
        for (int x = 0; x < 16; x++) {
          float t = VALUE_AT(mat, x, y);
          VALUE_AT(mat, x, y) = VALUE_AT(mat, 16 - x - 1, 16 - y - 1);
          VALUE_AT(mat, 16 - x - 1, 16 - y - 1) = t;
        }
      }
      break;
    case TemperatureRotation::ROTATION_270:
      for (int y = 0; y < 16 / 2; y++) {
        for (int x = y; x < 16 - y - 1; x++) {
          float t = VALUE_AT(mat, x, y);
          VALUE_AT(mat, x, y) = VALUE_AT(mat, 16 - y - 1, x);
          VALUE_AT(mat, 16 - y - 1, x) = VALUE_AT(mat, 16 - x - 1, 16 - y - 1);
          VALUE_AT(mat, 16 - x - 1, 16 - y - 1) = VALUE_AT(mat, y, 16 - x - 1);
          VALUE_AT(mat, y, 16 - x - 1) = t;
        }
      }
      break;
    default:
      break;
  }
}

bool TemperatureTask::try_reading(TemperatureMatrix &mat) {
  const int MAX_TRIAL = 10;
  const int INTERVAL = 200;
  int trial = 0;
  while (SZ_RETCODE_OK != temperature_reader_->read(mat) && ++trial < MAX_TRIAL)
    QThread::msleep(INTERVAL);

  if (mat.size != 256) {
    SZ_LOG_ERROR("only 16x16 is supported");
    return false;
  }

  bool ret = trial < MAX_TRIAL;
  if (ret) {
    QThread::msleep(INTERVAL);
    rotate_temperature_matrix(mat);
  }
  return ret;
}

void TemperatureTask::init_valid_temperature_area(DetectionRatio &detection) {
  detection.x = 0.45;
  detection.y = 0.45;
  detection.width = 0.1;
  detection.height = 0.1;
}

void TemperatureTask::get_valid_temperature_area(DetectionRatio &detection,
                                                 bool to_clear) {
  if (to_clear) {
    init_valid_temperature_area(detection);
  } else {
    auto cfg = Config::get_temperature();
    float x1 = (cfg.max_x - cfg.min_x) * detection.x + cfg.min_x;
    float x2 =
        (cfg.max_x - cfg.min_x) * (detection.x + detection.width) + cfg.min_x;
    detection.x = std::max(floor(x1 * 16) - 1, 0.f) / 16.f;
    detection.width = std::min(ceil(x2 * 16) + 1, 16.f) / 16.f - detection.x;

    float y1 = (cfg.max_y - cfg.min_y) * detection.y + cfg.min_y;
    float y2 =
        (cfg.max_y - cfg.min_y) * (detection.y + detection.height) + cfg.min_y;

    detection.y = std::max(floor((y1 - (y2 - y1) * .2f) * 16), 0.f) / 16.f;
    detection.height = std::max((y2 - y1) * .4f * 16, 2.f) / 16.f;
  }
}

void TemperatureTask::get_valid_temperature(const TemperatureMatrix &mat,
                                            const DetectionRatio &detection,
                                            std::vector<float> &statistics,
                                            float &max_x, float &max_y,
                                            float &max_temperature) {
  statistics.clear();
  for (size_t i = 0; i < 256; i++) {
    float x = (i % 16) / 16.f, y = (i / 16) / 16.f;
    if (pow(x - 0.5f, 2) + pow(y - 0.5f, 2) <= pow(6.f / 16.f, 2) &&
        detection.x <= x && x <= detection.x + detection.width &&
        detection.y <= y && y <= detection.y + detection.height) {
      statistics.push_back(mat.value[i]);
      if (mat.value[i] > max_temperature) {
        max_temperature = mat.value[i];
        max_x = x;
        max_y = y;
      }
    }
  }
}

bool TemperatureTask::get_final_temperature(
    const std::vector<float> &statistics, float max_temperature) {
  float avg = 0, var = 0;
  for (float v : statistics) avg += v / statistics.size();
  for (float v : statistics) var += (v - avg) * (v - avg);
  face_temperature_ = surface_to_inner(
      measure_to_surface(max_temperature + Config::get_temperature_bias()));

  if (current_var_ < 0)
    current_var_ = var;
  else
    current_var_ = current_var_ * .8f + var * .2f;

  if (!Config::enable_anti_spoofing() ||
      current_var_ > Config::get_user().temperature_var) {
    SZ_LOG_INFO("max={:.2f}°C, face={:.2f}°C, var={:.2f}°C", max_temperature,
                face_temperature_, current_var_);
    emit tx_temperature(face_temperature_);
    return true;
  }
  return false;
}

void TemperatureTask::rx_update(DetectionRatio detection, bool to_clear) {
  is_running_ = true;

  if (temperature_reader_ == nullptr) {
    connect();
  } else {
    while (!try_reading(temperature_matrix_))
      ;

    get_valid_temperature_area(detection, to_clear);

    float max_x, max_y, max_temperature;
    static std::vector<float> statistics;
    get_valid_temperature(temperature_matrix_, detection, statistics, max_x,
                          max_y, max_temperature);
    if (to_clear) {
      max_x = max_y = 0.5;
      if (ambient_temperature_ == 0)
        ambient_temperature_ = max_temperature;
      else
        ambient_temperature_ =
            ambient_temperature_ * 0.9 + max_temperature * 0.1;
    } else {
      if (!get_final_temperature(statistics, max_temperature)) {
        init_valid_temperature_area(detection);
        max_x = max_y = 0.5;
      }
    }

    static TemperatureMatrix output;
    if (output.value == nullptr) {
      output.value = (float *)malloc(temperature_matrix_.size * sizeof(float));
      output.size = temperature_matrix_.size;
    }
    memcpy(output.value, temperature_matrix_.value,
           output.size * sizeof(float));
    emit tx_heatmap(output, detection, max_x, max_y);
  }

  is_running_ = false;
}
