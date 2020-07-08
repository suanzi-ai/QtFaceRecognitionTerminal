#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <fstream>
#include <nlohmann/json.hpp>
#include <quface/common.hpp>
#include <string>

#include "logger.hpp"

#define LOAD_JSON_TO(config, key, value) \
  if (config.contains(key)) {            \
    config.at(key).get_to(value);        \
  }

typedef struct {
  std::string product_key;
  std::string device_name;
  std::string device_secret;
  std::string client_id;
  std::string db_name;
  std::string model_file_path;
  std::string license_filename;
} QufaceConfig;

typedef struct {
  int index;
  int rotate;
  int flip;
  int min_face_height;
  int min_face_width;
  int max_window_height;
  int max_window_width;
  int target_area_width_percent;
  int target_area_height_percent;
} CameraConfig;

typedef struct {
  SZ_INT32 min_history_size;
  SZ_INT32 max_history_size;
  SZ_INT32 min_recognized_count;
  SZ_FLOAT min_recognized_score;
  SZ_FLOAT min_accumulate_score;
  SZ_INT32 max_lost_age;
} ExtractConfig;

typedef struct {
  int queue_size;
  int min_alive;
  int continuous_max_missed;
  int max_no_face;
} LivenessConfig;

namespace suanzi {
class Config {
 public:
  typedef std::shared_ptr<Config> ptr;

  template <typename... Args>
  static ptr make_shared(Args &&... args) {
    return std::make_shared<Config>(std::forward<Args>(args)...);
  }

  SZ_RETCODE load(const std::string &filename) {
    std::ifstream i(filename);
    if (!i.is_open()) {
      SZ_LOG_WARN("{} not present, will using defaults", filename);
      return SZ_RETCODE_OK;
    }

    nlohmann::json config;
    i >> config;

    if (config.contains("app")) {
      auto _a = config["app"];
      LOAD_JSON_TO(_a, "server_port", server_port);
      LOAD_JSON_TO(_a, "image_store_path", image_store_path);
      LOAD_JSON_TO(_a, "person_service_base_url", person_service_base_url);
      LOAD_JSON_TO(_a, "unknown_avatar_image_path", unknown_avatar_image_path);
    }

    if (config.contains("quface")) {
      auto _q = config["quface"];
      LOAD_JSON_TO(_q, "product_key", quface.product_key);
      LOAD_JSON_TO(_q, "device_name", quface.device_name);
      LOAD_JSON_TO(_q, "device_secret", quface.device_secret);
      LOAD_JSON_TO(_q, "client_id", quface.client_id);
      LOAD_JSON_TO(_q, "db_name", quface.db_name);
      LOAD_JSON_TO(_q, "model_file_path", quface.model_file_path);
      LOAD_JSON_TO(_q, "license_filename", quface.license_filename);
    }

    if (config.contains("cameras")) {
      auto _c = config["cameras"];

      if (_c.contains("normal")) {
        auto _n = _c["normal"];
        LOAD_JSON_TO(_n, "index", normal.index);
        LOAD_JSON_TO(_n, "rotate", normal.rotate);
        LOAD_JSON_TO(_n, "flip", normal.flip);
        LOAD_JSON_TO(_n, "min_face_height", normal.min_face_height);
        LOAD_JSON_TO(_n, "min_face_width", normal.min_face_width);
        LOAD_JSON_TO(_n, "max_window_height", normal.max_window_height);
        LOAD_JSON_TO(_n, "max_window_width", normal.max_window_width);
        LOAD_JSON_TO(_n, "target_area_width_percent",
                     normal.target_area_width_percent);
        LOAD_JSON_TO(_n, "target_area_height_percent",
                     normal.target_area_height_percent);
      }

      if (_c.contains("infrared")) {
        auto _i = _c["infrared"];
        LOAD_JSON_TO(_i, "index", infrared.index);
        LOAD_JSON_TO(_i, "rotate", infrared.rotate);
        LOAD_JSON_TO(_i, "flip", infrared.flip);
        LOAD_JSON_TO(_i, "min_face_height", infrared.min_face_height);
        LOAD_JSON_TO(_i, "min_face_width", infrared.min_face_width);
        LOAD_JSON_TO(_i, "max_window_height", infrared.max_window_height);
        LOAD_JSON_TO(_i, "max_window_width", infrared.max_window_width);
        LOAD_JSON_TO(_i, "target_area_width_percent",
                     infrared.target_area_width_percent);
        LOAD_JSON_TO(_i, "target_area_height_percent",
                     infrared.target_area_height_percent);
      }
    }

    if (config.contains("liveness")) {
      auto _l = config["liveness"];
      LOAD_JSON_TO(_l, "queue_size", liveness.queue_size);
      LOAD_JSON_TO(_l, "min_alive", liveness.min_alive);
      LOAD_JSON_TO(_l, "continuous_max_missed", liveness.continuous_max_missed);
      LOAD_JSON_TO(_l, "max_no_face", liveness.max_no_face);
    }

    if (config.contains("extract")) {
      auto _e = config["extract"];
      LOAD_JSON_TO(_e, "min_history_size", extract.min_history_size);
      LOAD_JSON_TO(_e, "max_history_size", extract.max_history_size);
      LOAD_JSON_TO(_e, "min_recognized_count", extract.min_recognized_count);
      LOAD_JSON_TO(_e, "min_recognized_score", extract.min_recognized_score);
      LOAD_JSON_TO(_e, "min_accumulate_score", extract.min_accumulate_score);
      LOAD_JSON_TO(_e, "max_lost_age", extract.max_lost_age);
    }

    return SZ_RETCODE_OK;
  }

  bool has_license_info() {
    return quface.product_key.size() > 0 && quface.device_name.size() > 0 &&
           quface.device_secret.size() > 0;
  }

  SZ_UINT16 server_port = 8010;
  std::string image_store_path = "/user/go-app/upload/";
  std::string person_service_base_url = "http://localhost:8008";
  std::string unknown_avatar_image_path = "avatar_unknown.jpg";

  QufaceConfig quface = {
      .product_key = "",
      .device_name = "",
      .device_secret = "",
      .client_id = "quface_qt",
      .db_name = "quface",
      .model_file_path = "facemodel.bin",
      .license_filename = "license.json",
  };

  CameraConfig normal = {
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

  CameraConfig infrared = {
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

  ExtractConfig extract = {
      .min_history_size = 3,
      .max_history_size = 16,
      .min_recognized_count = 3,
      .min_recognized_score = .5f,
      .min_accumulate_score = 2.7f,
      .max_lost_age = 10,
  };

  LivenessConfig liveness = {
      .queue_size = 16,
      .min_alive = 7,
      .continuous_max_missed = 3,
      .max_no_face = 3,
  };
};
}  // namespace suanzi

#endif
