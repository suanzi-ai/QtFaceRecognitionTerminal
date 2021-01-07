#include "matrix_temperature_app.hpp"

#include <cmath>
#include <quface-io/engine.hpp>

#include "config.hpp"
#include "temperature_task.hpp"

#define VALUE_AT(m, x, y) (((m).value)[(y) * (m).width + (x)])
#define IS_NULL(t) (std::abs((t)-INVALID_TEMPERATURE) < 0.001)

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

MatrixTemperatureApp::MatrixTemperatureApp(TemperatureTask *temperature_task)
    : TemperatureApp(temperature_task) {
  current_variance_ = -1.0;
}

bool MatrixTemperatureApp::init() {
  if (!TemperatureApp::init()) return false;

  int success = 0;
  while (temperature_reader_ != nullptr && success < 10) {
    if (try_read()) {
      success++;
      emit temperature_task_->tx_heatmap_init(success);
      QThread::msleep(500);
    }
  }
  return true;
}

bool MatrixTemperatureApp::try_read() {
  const int MAX_TRIAL = 10;
  const int INTERVAL = 200;
  int trial = 0;
  while (SZ_RETCODE_OK != temperature_reader_->read(temperatures_) &&
         ++trial < MAX_TRIAL)
    QThread::msleep(INTERVAL);

  if (temperatures_.size != 256 && temperatures_.size != 1024) {
    SZ_LOG_ERROR("only 16x16 and 32x32 is supported");
    return false;
  }

  bool ret = trial < MAX_TRIAL;
  if (ret) {
    rotate_temperature_matrix(temperatures_);
  }
  return ret;
}

void MatrixTemperatureApp::read_temperature(const QRectF &face_area,
                                            bool valid_face_area) {
  if (temperature_reader_ == nullptr) {
    init();
    return;
  }

  while (!try_read())
    ;

  face_area_to_temperature_area(face_area, valid_face_area, temperature_area_);

  float max_x, max_y, max_temperature;
  static std::vector<float> statistics;

  get_valid_temperature(statistics, max_x, max_y, max_temperature);
  if (valid_face_area) {
    if (!get_face_temperature(statistics, max_temperature)) {
      init_temperature_area(temperature_area_);
      max_x = max_y = 0.5;
    }

    // printf("body temp(%d, %d)=%.2f max temp(%.1f, %.1f)=%.2f\n",
    // temperatures_.body_temperature_x, temperatures_.body_temperature_y,
    // temperatures_.body_temperature, max_x*32, max_y*32, max_temperature);
  } else {
    current_variance_ = -1;
    max_x = max_y = 0.5;
    if (ambient_temperature_ == 0)
      ambient_temperature_ = max_temperature;
    else
      ambient_temperature_ = ambient_temperature_ * 0.9 + max_temperature * 0.1;
  }

  emit temperature_task_->tx_heatmap(temperatures_, temperature_area_, max_x,
                                     max_y);
}

void MatrixTemperatureApp::get_valid_temperature(std::vector<float> &statistics,
                                                 float &max_x, float &max_y,
                                                 float &max_temperature) {
  statistics.clear();
  max_temperature = 0.0;
  auto cfg = Config::get_temperature();
  for (int i = 0; i < temperatures_.size; i++) {
    float x = (i % temperatures_.width) * 1.0 / temperatures_.width,
          y = (i / temperatures_.height) * 1.0 / temperatures_.height;
    if (pow(x - 0.5f, 2) + pow(y - 0.5f, 2) <=
            pow(cfg.temperature_area_radius, 2) &&
        temperature_area_.x() <= x &&
        x <= temperature_area_.x() + temperature_area_.width() &&
        temperature_area_.y() <= y &&
        y <= temperature_area_.y() + temperature_area_.height()) {
      statistics.push_back(temperatures_.value[i]);
      if (temperatures_.value[i] > max_temperature) {
        max_temperature = temperatures_.value[i];
        max_x = x;
        max_y = y;
      }
    }
  }
}

float MatrixTemperatureApp::get_valid_temperature_variance(
    const std::vector<float> &statistics) {
  float avg = 0, var = 0;
  for (float v : statistics) avg += v / statistics.size();
  for (float v : statistics) var += (v - avg) * (v - avg);

  if (current_variance_ < 0.0)
    current_variance_ = var;
  else
    current_variance_ = current_variance_ * .8f + var * .2f;
  return current_variance_;
}

bool MatrixTemperatureApp::get_face_temperature(
    const std::vector<float> &statistics, float max_temperature) {
  float face_temperature = 0.0;

  switch (Config::get_temperature().manufacturer) {
    case TemperatureManufacturer::Otpa:
      face_temperature = surface_to_inner(
          measure_to_surface(max_temperature + Config::get_temperature_bias()));
      break;
    case TemperatureManufacturer::Haiman_NdkF4l5:
      face_temperature = max_temperature;
      break;
    default:
      face_temperature = max_temperature;
      break;
  }

  float current_var = get_valid_temperature_variance(statistics);
  if (!Config::enable_anti_spoofing() ||
      current_var > Config::get_user().temperature_var) {
    SZ_LOG_INFO("max={:.2f}°C, face={:.2f}°C, var={:.2f}°C", max_temperature,
                face_temperature, current_var);
    emit temperature_task_->tx_temperature(face_temperature);
    return true;
  }
  return false;
}

void MatrixTemperatureApp::rotate_temperature_matrix(TemperatureMatrix &mat) {
  if (mat.width != mat.height) return;

  auto cfg = Config::get_temperature();
  switch (cfg.sensor_rotation) {
    case TemperatureRotation::ROTATION_90:
      for (int y = 0; y < mat.height / 2; y++) {
        for (int x = y; x < mat.width - y - 1; x++) {
          float t = VALUE_AT(mat, x, y);
          VALUE_AT(mat, x, y) = VALUE_AT(mat, y, mat.width - x - 1);
          VALUE_AT(mat, y, mat.width - x - 1) =
              VALUE_AT(mat, mat.width - x - 1, mat.width - y - 1);
          VALUE_AT(mat, mat.width - x - 1, mat.width - y - 1) =
              VALUE_AT(mat, mat.width - y - 1, x);
          VALUE_AT(mat, mat.width - y - 1, x) = t;
        }
      }
      break;
    case TemperatureRotation::ROTATION_180:
      for (int y = 0; y < mat.height / 2; y++) {
        for (int x = 0; x < mat.width; x++) {
          float t = VALUE_AT(mat, x, y);
          VALUE_AT(mat, x, y) =
              VALUE_AT(mat, mat.width - x - 1, mat.width - y - 1);
          VALUE_AT(mat, mat.width - x - 1, mat.width - y - 1) = t;
        }
      }
      break;
    case TemperatureRotation::ROTATION_270:
      for (int y = 0; y < mat.height / 2; y++) {
        for (int x = y; x < mat.width - y - 1; x++) {
          float t = VALUE_AT(mat, x, y);
          VALUE_AT(mat, x, y) = VALUE_AT(mat, mat.width - y - 1, x);
          VALUE_AT(mat, mat.width - y - 1, x) =
              VALUE_AT(mat, mat.width - x - 1, mat.width - y - 1);
          VALUE_AT(mat, mat.width - x - 1, mat.width - y - 1) =
              VALUE_AT(mat, y, mat.width - x - 1);
          VALUE_AT(mat, y, mat.width - x - 1) = t;
        }
      }
      break;
    default:
      break;
  }
}

void MatrixTemperatureApp::init_temperature_area(QRectF &temperature_area) {
  temperature_area.setRect(0.45, 0.45, 0.1, 0.1);
}

void MatrixTemperatureApp::face_area_to_temperature_area(
    const QRectF &face_area, bool valid_face_area, QRectF &temperature_area) {
  if (valid_face_area) {
    auto cfg = Config::get_temperature();

    // face area map to temperature area
    float x1 =
        cfg.temperature_area_width * face_area.x() + cfg.temperature_area_x;
    float x2 =
        cfg.temperature_area_width * (face_area.x() + face_area.width()) +
        cfg.temperature_area_x;

    temperature_area.setX(std::max(floor(x1 * temperatures_.width) - 1, 0.f) /
                          temperatures_.width);
    temperature_area.setWidth(std::min(ceil(x2 * temperatures_.width) + 1,
                                       (float)temperatures_.width) /
                                  (float)temperatures_.width -
                              temperature_area.x());

    float y1 =
        cfg.temperature_area_height * face_area.y() + cfg.temperature_area_y;
    float y2 =
        cfg.temperature_area_height * (face_area.y() + face_area.height()) +
        cfg.temperature_area_y;

    temperature_area.setY(
        std::max(floor((y1 - (y2 - y1) * .2f) * temperatures_.height), 0.f) /
        temperatures_.height);
    temperature_area.setHeight(
        std::max(ceil((y2 - y1) * .4f * temperatures_.height), 2.f) /
        temperatures_.height);
  } else {
    init_temperature_area(temperature_area);
  }
}
