#include "config.hpp"

using namespace suanzi;

#define LOAD_JSON_TO(config, key, value) \
  if (config.contains(key)) {            \
    config.at(key).get_to(value);        \
  }

#define SAVE_JSON_TO(config, key, value) j[key] = value;

void suanzi::to_json(json &j, const AppConfig &c) {
  SAVE_JSON_TO(j, "server_port", c.server_port);
  SAVE_JSON_TO(j, "server_host", c.server_host);
  SAVE_JSON_TO(j, "image_store_path", c.image_store_path);
  SAVE_JSON_TO(j, "person_service_base_url", c.person_service_base_url);
}

void suanzi::from_json(const json &j, AppConfig &c) {
  LOAD_JSON_TO(j, "server_port", c.server_port);
  LOAD_JSON_TO(j, "server_host", c.server_host);
  LOAD_JSON_TO(j, "image_store_path", c.image_store_path);
  LOAD_JSON_TO(j, "person_service_base_url", c.person_service_base_url);
}

void suanzi::to_json(json &j, const QufaceConfig &c) {
  SAVE_JSON_TO(j, "product_key", c.product_key);
  SAVE_JSON_TO(j, "device_name", c.device_name);
  SAVE_JSON_TO(j, "device_secret", c.device_secret);
  SAVE_JSON_TO(j, "client_id", c.client_id);
  SAVE_JSON_TO(j, "db_name", c.db_name);
  SAVE_JSON_TO(j, "model_file_path", c.model_file_path);
  SAVE_JSON_TO(j, "license_filename", c.license_filename);
}

void suanzi::from_json(const json &j, QufaceConfig &c) {
  LOAD_JSON_TO(j, "product_key", c.product_key);
  LOAD_JSON_TO(j, "device_name", c.device_name);
  LOAD_JSON_TO(j, "device_secret", c.device_secret);
  LOAD_JSON_TO(j, "client_id", c.client_id);
  LOAD_JSON_TO(j, "db_name", c.db_name);
  LOAD_JSON_TO(j, "model_file_path", c.model_file_path);
  LOAD_JSON_TO(j, "license_filename", c.license_filename);
}

void suanzi::to_json(json &j, const CameraConfig &c) {
  SAVE_JSON_TO(j, "index", c.index);
  SAVE_JSON_TO(j, "rotate", c.rotate);
  SAVE_JSON_TO(j, "flip", c.flip);
  SAVE_JSON_TO(j, "min_face_height", c.min_face_height);
  SAVE_JSON_TO(j, "min_face_width", c.min_face_width);
  SAVE_JSON_TO(j, "max_window_height", c.max_window_height);
  SAVE_JSON_TO(j, "max_window_width", c.max_window_width);
  SAVE_JSON_TO(j, "target_area_width_percent", c.target_area_width_percent);
  SAVE_JSON_TO(j, "target_area_height_percent", c.target_area_height_percent);
}

void suanzi::from_json(const json &j, CameraConfig &c) {
  LOAD_JSON_TO(j, "index", c.index);
  LOAD_JSON_TO(j, "rotate", c.rotate);
  LOAD_JSON_TO(j, "flip", c.flip);
  LOAD_JSON_TO(j, "min_face_height", c.min_face_height);
  LOAD_JSON_TO(j, "min_face_width", c.min_face_width);
  LOAD_JSON_TO(j, "max_window_height", c.max_window_height);
  LOAD_JSON_TO(j, "max_window_width", c.max_window_width);
  LOAD_JSON_TO(j, "target_area_width_percent", c.target_area_width_percent);
  LOAD_JSON_TO(j, "target_area_height_percent", c.target_area_height_percent);
}

void suanzi::to_json(json &j, const DetectConfig &c) {
  SAVE_JSON_TO(j, "threshold", c.threshold);
  SAVE_JSON_TO(j, "min_face_size", c.min_face_size);
}

void suanzi::from_json(const json &j, DetectConfig &c) {
  LOAD_JSON_TO(j, "threshold", c.threshold);
  LOAD_JSON_TO(j, "min_face_size", c.min_face_size);
}

void suanzi::to_json(json &j, const ExtractConfig &c) {
  SAVE_JSON_TO(j, "history_size", c.history_size);
  SAVE_JSON_TO(j, "min_recognize_count", c.min_recognize_count);
  SAVE_JSON_TO(j, "min_recognize_score", c.min_recognize_score);
  SAVE_JSON_TO(j, "min_accumulate_score", c.min_accumulate_score);
  SAVE_JSON_TO(j, "max_lost_age", c.max_lost_age);
}

void suanzi::from_json(const json &j, ExtractConfig &c) {
  LOAD_JSON_TO(j, "history_size", c.history_size);
  LOAD_JSON_TO(j, "min_recognize_count", c.min_recognize_count);
  LOAD_JSON_TO(j, "min_recognize_score", c.min_recognize_score);
  LOAD_JSON_TO(j, "min_accumulate_score", c.min_accumulate_score);
  LOAD_JSON_TO(j, "max_lost_age", c.max_lost_age);
}

void suanzi::to_json(json &j, const LivenessConfig &c) {
  SAVE_JSON_TO(j, "queue_size", c.queue_size);
  SAVE_JSON_TO(j, "min_alive", c.min_alive);
  SAVE_JSON_TO(j, "continuous_max_missed", c.continuous_max_missed);
  SAVE_JSON_TO(j, "max_no_face", c.max_no_face);
}

void suanzi::from_json(const json &j, LivenessConfig &c) {
  LOAD_JSON_TO(j, "queue_size", c.queue_size);
  LOAD_JSON_TO(j, "min_alive", c.min_alive);
  LOAD_JSON_TO(j, "continuous_max_missed", c.continuous_max_missed);
  LOAD_JSON_TO(j, "max_no_face", c.max_no_face);
}

void suanzi::from_json(const json &j, Config &c) {
  LOAD_JSON_TO(j, "app", c.app);
  LOAD_JSON_TO(j, "quface", c.quface);
  LOAD_JSON_TO(j, "detect", c.detect);
  LOAD_JSON_TO(j, "extract", c.extract);
  LOAD_JSON_TO(j, "liveness", c.liveness);

  if (j.contains("cameras")) {
    LOAD_JSON_TO(j.at("cameras"), "normal", c.normal);
    LOAD_JSON_TO(j.at("cameras"), "infrared", c.infrared);
  }
}

void suanzi::to_json(json &j, const Config &c) {
  SAVE_JSON_TO(j, "app", c.app);
  SAVE_JSON_TO(j, "quface", c.quface);
  SAVE_JSON_TO(j, "detect", c.detect);
  SAVE_JSON_TO(j, "extract", c.extract);
  SAVE_JSON_TO(j, "liveness", c.liveness);

  json cameras;
  SAVE_JSON_TO(cameras, "normal", c.normal);
  SAVE_JSON_TO(cameras, "infrared", c.infrared);
  SAVE_JSON_TO(j, "cameras", cameras);
}

SZ_RETCODE Config::load_defaults() {
  app = {
      .server_port = 8010,
      .server_host = "0.0.0.0",
      .image_store_path = "/user/go-app/upload/",
      .person_service_base_url = "http://localhost:8008",
  };

  quface = {
      .product_key = "",
      .device_name = "",
      .device_secret = "",
      .client_id = "quface_qt",
      .db_name = "quface",
      .model_file_path = "facemodel.bin",
      .license_filename = "license.json",
  };

  normal = {
      .index = 1,
      .rotate = 0,
      .flip = -2,
      .min_face_height = 100,
      .min_face_width = 100,
      .max_window_height = 800,
      .max_window_width = 600,
      .target_area_width_percent = 60,
      .target_area_height_percent = 60,
  };

  infrared = {
      .index = 1,
      .rotate = 0,
      .flip = -2,
      .min_face_height = 100,
      .min_face_width = 100,
      .max_window_height = 800,
      .max_window_width = 600,
      .target_area_width_percent = 60,
      .target_area_height_percent = 60,
  };

  detect = {
      .threshold = 0.4f,
      .min_face_size = 40,
  };

  extract = {
      .history_size = 15,
      .min_recognize_count = 10,
      .min_recognize_score = .75f,
      .min_accumulate_score = 7.0f,
      .max_lost_age = 20,
  };

  liveness = {
      .queue_size = 16,
      .min_alive = 7,
      .continuous_max_missed = 3,
      .max_no_face = 3,
  };
}

SZ_RETCODE Config::load() {
  std::ifstream i(config_file_);
  if (!i.is_open()) {
    SZ_LOG_WARN("{} not present, will using defaults", config_file_);
    return SZ_RETCODE_OK;
  }

  json config;
  i >> config;

  config.get_to(*this);

  return SZ_RETCODE_OK;
}

SZ_RETCODE Config::save() {
  json config = json(*this);

  std::ofstream o(config_file_);
  if (!o.is_open()) {
    SZ_LOG_WARN("Open {} failed, can't save", config_file_);
    return SZ_RETCODE_FAILED;
  }

  o << config.dump();

  return SZ_RETCODE_OK;
}
