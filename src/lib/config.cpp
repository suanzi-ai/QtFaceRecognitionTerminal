#include "config.hpp"

#include <QFile>
#include <regex>

using namespace suanzi;
using namespace suanzi::io;

void suanzi::to_json(json &j, const UserConfig &c) {
  SAVE_JSON_TO(j, "lang", c.lang);
  SAVE_JSON_TO(j, "blacklist_policy", c.blacklist_policy);
  SAVE_JSON_TO(j, "liveness_policy", c.liveness_policy);
  SAVE_JSON_TO(j, "detect_level", c.detect_level);
  SAVE_JSON_TO(j, "extract_level", c.extract_level);
  SAVE_JSON_TO(j, "liveness_level", c.liveness_level);
  SAVE_JSON_TO(j, "duplication_interval", c.duplication_interval);
  SAVE_JSON_TO(j, "duplication_limit", c.duplication_limit);
  SAVE_JSON_TO(j, "relay_switch_cond", c.relay_switch_cond);
  SAVE_JSON_TO(j, "relay_default_state", c.relay_default_state);
  SAVE_JSON_TO(j, "relay_restore_time", c.relay_restore_time);
  SAVE_JSON_TO(j, "relay_switch_mode", c.relay_switch_mode);
  SAVE_JSON_TO(j, "enable_temperature", c.enable_temperature);
  SAVE_JSON_TO(j, "enable_anti_spoofing", c.enable_anti_spoofing);
  SAVE_JSON_TO(j, "temperature_bias", c.temperature_bias);
  SAVE_JSON_TO(j, "temperature_finetune", c.temperature_finetune);
  SAVE_JSON_TO(j, "temperature_var", c.temperature_var);
  SAVE_JSON_TO(j, "temperature_max", c.temperature_max);
  SAVE_JSON_TO(j, "temperature_min", c.temperature_min);
  SAVE_JSON_TO(j, "enable_audio", c.enable_audio);
  SAVE_JSON_TO(j, "enable_record_audio", c.enable_record_audio);
  SAVE_JSON_TO(j, "enable_temperature_audio", c.enable_temperature_audio);
  SAVE_JSON_TO(j, "enable_mask_audio", c.enable_mask_audio);
  SAVE_JSON_TO(j, "enable_distance_audio", c.enable_distance_audio);
  SAVE_JSON_TO(j, "enable_pass_audio", c.enable_pass_audio);
  SAVE_JSON_TO(j, "enable_led", c.enable_led);
  SAVE_JSON_TO(j, "enable_screensaver", c.enable_screensaver);
  SAVE_JSON_TO(j, "enable_co2", c.enable_co2);
  SAVE_JSON_TO(j, "enable_read_card", c.enable_read_card);
  SAVE_JSON_TO(j, "screensaver_timeout", c.screensaver_timeout);
  SAVE_JSON_TO(j, "upload_known_person", c.upload_known_person);
  SAVE_JSON_TO(j, "upload_unknown_person", c.upload_unknown_person);
  SAVE_JSON_TO(j, "upload_hd_snapshot", c.upload_hd_snapshot);
  SAVE_JSON_TO(j, "mask_score", c.mask_score);
  SAVE_JSON_TO(j, "ir_validate_score", c.ir_validate_score);
  SAVE_JSON_TO(j, "bgr_validate_score", c.bgr_validate_score);
  SAVE_JSON_TO(j, "wdr", c.wdr);
}

void suanzi::from_json(const json &j, UserConfig &c) {
  LOAD_JSON_TO(j, "lang", c.lang);
  LOAD_JSON_TO(j, "blacklist_policy", c.blacklist_policy);
  LOAD_JSON_TO(j, "liveness_policy", c.liveness_policy);
  LOAD_JSON_TO(j, "detect_level", c.detect_level);
  LOAD_JSON_TO(j, "extract_level", c.extract_level);
  LOAD_JSON_TO(j, "liveness_level", c.liveness_level);
  LOAD_JSON_TO(j, "duplication_interval", c.duplication_interval);
  LOAD_JSON_TO(j, "duplication_limit", c.duplication_limit);
  LOAD_JSON_TO(j, "relay_switch_cond", c.relay_switch_cond);
  LOAD_JSON_TO(j, "relay_default_state", c.relay_default_state);
  LOAD_JSON_TO(j, "relay_restore_time", c.relay_restore_time);
  LOAD_JSON_TO(j, "relay_switch_mode", c.relay_switch_mode);
  if (j.contains("disabled_temperature")) {
    bool disabled_temperature;
    LOAD_JSON_TO(j, "disabled_temperature", disabled_temperature);
    c.enable_temperature = !disabled_temperature;
  } else {
    LOAD_JSON_TO(j, "enable_temperature", c.enable_temperature);
  }
  LOAD_JSON_TO(j, "enable_anti_spoofing", c.enable_anti_spoofing);
  LOAD_JSON_TO(j, "temperature_bias", c.temperature_bias);
  LOAD_JSON_TO(j, "temperature_finetune", c.temperature_finetune);
  LOAD_JSON_TO(j, "temperature_var", c.temperature_var);
  LOAD_JSON_TO(j, "temperature_max", c.temperature_max);
  LOAD_JSON_TO(j, "temperature_min", c.temperature_min);
  LOAD_JSON_TO(j, "enable_audio", c.enable_audio);
  LOAD_JSON_TO(j, "enable_record_audio", c.enable_record_audio);
  LOAD_JSON_TO(j, "enable_temperature_audio", c.enable_temperature_audio);
  LOAD_JSON_TO(j, "enable_mask_audio", c.enable_mask_audio);
  LOAD_JSON_TO(j, "enable_distance_audio", c.enable_distance_audio);
  LOAD_JSON_TO(j, "enable_pass_audio", c.enable_pass_audio);
  LOAD_JSON_TO(j, "enable_led", c.enable_led);
  LOAD_JSON_TO(j, "enable_screensaver", c.enable_screensaver);
  LOAD_JSON_TO(j, "enable_co2", c.enable_co2);
  LOAD_JSON_TO(j, "enable_read_card", c.enable_read_card);
  LOAD_JSON_TO(j, "screensaver_timeout", c.screensaver_timeout);
  LOAD_JSON_TO(j, "upload_known_person", c.upload_known_person);
  LOAD_JSON_TO(j, "upload_unknown_person", c.upload_unknown_person);
  LOAD_JSON_TO(j, "upload_hd_snapshot", c.upload_hd_snapshot);
  LOAD_JSON_TO(j, "mask_score", c.mask_score);
  LOAD_JSON_TO(j, "ir_validate_score", c.ir_validate_score);
  LOAD_JSON_TO(j, "bgr_validate_score", c.bgr_validate_score);
  LOAD_JSON_TO(j, "wdr", c.wdr);
}

void suanzi::to_json(json &j, const AppConfig &c) {
  SAVE_JSON_TO(j, "recognize_tip_top_percent", c.recognize_tip_top_percent);
  SAVE_JSON_TO(j, "server_port", c.server_port);
  SAVE_JSON_TO(j, "server_host", c.server_host);
  SAVE_JSON_TO(j, "image_store_path", c.image_store_path);
  SAVE_JSON_TO(j, "person_service_base_url", c.person_service_base_url);
  SAVE_JSON_TO(j, "show_infrared_window", c.show_infrared_window);
  SAVE_JSON_TO(j, "infrared_window_percent", c.infrared_window_percent);
  SAVE_JSON_TO(j, "show_isp_info_window", c.show_isp_info_window);
  SAVE_JSON_TO(j, "boot_image_path", c.boot_image_path);
  SAVE_JSON_TO(j, "screensaver_image_path", c.screensaver_image_path);
  SAVE_JSON_TO(j, "has_touch_screen", c.has_touch_screen);
}

void suanzi::from_json(const json &j, AppConfig &c) {
  LOAD_JSON_TO(j, "recognize_tip_top_percent", c.recognize_tip_top_percent);
  LOAD_JSON_TO(j, "server_port", c.server_port);
  LOAD_JSON_TO(j, "server_host", c.server_host);
  LOAD_JSON_TO(j, "image_store_path", c.image_store_path);
  LOAD_JSON_TO(j, "person_service_base_url", c.person_service_base_url);
  LOAD_JSON_TO(j, "show_infrared_window", c.show_infrared_window);
  LOAD_JSON_TO(j, "infrared_window_percent", c.infrared_window_percent);
  LOAD_JSON_TO(j, "show_isp_info_window", c.show_isp_info_window);
  LOAD_JSON_TO(j, "boot_image_path", c.boot_image_path);
  LOAD_JSON_TO(j, "screensaver_image_path", c.screensaver_image_path);
  LOAD_JSON_TO(j, "has_touch_screen", c.has_touch_screen);
}

void suanzi::to_json(json &j, const TemperatureConfig &c) {
  SAVE_JSON_TO(j, "temperature_area_debug", c.temperature_area_debug);
  SAVE_JSON_TO(j, "detect_area_x", c.detect_area_x);
  SAVE_JSON_TO(j, "detect_area_y", c.detect_area_y);
  SAVE_JSON_TO(j, "detect_area_width", c.detect_area_width);
  SAVE_JSON_TO(j, "detect_area_height", c.detect_area_height);
  SAVE_JSON_TO(j, "min_face_height", c.min_face_height);
  SAVE_JSON_TO(j, "min_face_width", c.min_face_width);
  SAVE_JSON_TO(j, "manufacturer", c.manufacturer);
  SAVE_JSON_TO(j, "min_size", c.min_size);
  SAVE_JSON_TO(j, "temperature_type", c.temperature_type);
  SAVE_JSON_TO(j, "sensor_rotation", c.sensor_rotation);
  SAVE_JSON_TO(j, "temperature_area_x", c.temperature_area_x);
  SAVE_JSON_TO(j, "temperature_area_y", c.temperature_area_y);
  SAVE_JSON_TO(j, "temperature_area_width", c.temperature_area_width);
  SAVE_JSON_TO(j, "temperature_area_height", c.temperature_area_height);
  SAVE_JSON_TO(j, "temperature_area_radius", c.temperature_area_radius);
}

void suanzi::from_json(const json &j, TemperatureConfig &c) {
  LOAD_JSON_TO(j, "temperature_area_debug", c.temperature_area_debug);
  LOAD_JSON_TO(j, "detect_area_x", c.detect_area_x);
  LOAD_JSON_TO(j, "detect_area_y", c.detect_area_y);
  LOAD_JSON_TO(j, "detect_area_width", c.detect_area_width);
  LOAD_JSON_TO(j, "detect_area_height", c.detect_area_height);
  LOAD_JSON_TO(j, "min_face_height", c.min_face_height);
  LOAD_JSON_TO(j, "min_face_width", c.min_face_width);
  LOAD_JSON_TO(j, "manufacturer", c.manufacturer);
  LOAD_JSON_TO(j, "min_size", c.min_size);
  LOAD_JSON_TO(j, "temperature_type", c.temperature_type);
  LOAD_JSON_TO(j, "sensor_rotation", c.sensor_rotation);
  LOAD_JSON_TO(j, "temperature_area_x", c.temperature_area_x);
  LOAD_JSON_TO(j, "temperature_area_y", c.temperature_area_y);
  LOAD_JSON_TO(j, "temperature_area_width", c.temperature_area_width);
  LOAD_JSON_TO(j, "temperature_area_height", c.temperature_area_height);
  LOAD_JSON_TO(j, "temperature_area_radius", c.temperature_area_radius);
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
}

void suanzi::from_json(const json &j, CameraConfig &c) {
  LOAD_JSON_TO(j, "index", c.index);
  LOAD_JSON_TO(j, "rotate", c.rotate);
  LOAD_JSON_TO(j, "flip", c.flip);
}

void suanzi::to_json(json &j, const DetectConfig &c) {
  SAVE_JSON_TO(j, "threshold", c.threshold);
  SAVE_JSON_TO(j, "min_face_size", c.min_face_size);
  SAVE_JSON_TO(j, "max_yaw", c.max_yaw);
  SAVE_JSON_TO(j, "min_yaw", c.min_yaw);
  SAVE_JSON_TO(j, "max_pitch", c.max_pitch);
  SAVE_JSON_TO(j, "min_pitch", c.min_pitch);
  SAVE_JSON_TO(j, "max_roll", c.max_roll);
  SAVE_JSON_TO(j, "min_roll", c.min_roll);
  SAVE_JSON_TO(j, "min_tracking_iou", c.min_tracking_iou);
  SAVE_JSON_TO(j, "min_tracking_number", c.min_tracking_number);
}

void suanzi::from_json(const json &j, DetectConfig &c) {
  LOAD_JSON_TO(j, "threshold", c.threshold);
  LOAD_JSON_TO(j, "min_face_size", c.min_face_size);
  LOAD_JSON_TO(j, "max_yaw", c.max_yaw);
  LOAD_JSON_TO(j, "min_yaw", c.min_yaw);
  LOAD_JSON_TO(j, "max_pitch", c.max_pitch);
  LOAD_JSON_TO(j, "min_pitch", c.min_pitch);
  LOAD_JSON_TO(j, "max_roll", c.max_roll);
  LOAD_JSON_TO(j, "min_roll", c.min_roll);
  LOAD_JSON_TO(j, "min_tracking_iou", c.min_tracking_iou);
  LOAD_JSON_TO(j, "min_tracking_number", c.min_tracking_number);
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
  SAVE_JSON_TO(j, "history_size", c.history_size);
  SAVE_JSON_TO(j, "min_alive_count", c.min_alive_count);
  SAVE_JSON_TO(j, "continuous_max_lost_count", c.continuous_max_lost_count);
  SAVE_JSON_TO(j, "min_iou_between_bgr", c.min_iou_between_bgr);
  SAVE_JSON_TO(j, "min_width_ratio_between_bgr", c.min_width_ratio_between_bgr);
  SAVE_JSON_TO(j, "max_width_ratio_between_bgr", c.max_width_ratio_between_bgr);
  SAVE_JSON_TO(j, "min_height_ratio_between_bgr",
               c.min_height_ratio_between_bgr);
  SAVE_JSON_TO(j, "max_height_ratio_between_bgr",
               c.max_height_ratio_between_bgr);
}

void suanzi::from_json(const json &j, LivenessConfig &c) {
  LOAD_JSON_TO(j, "history_size", c.history_size);
  LOAD_JSON_TO(j, "min_alive_count", c.min_alive_count);
  LOAD_JSON_TO(j, "continuous_max_lost_count", c.continuous_max_lost_count);
  LOAD_JSON_TO(j, "min_iou_between_bgr", c.min_iou_between_bgr);
  LOAD_JSON_TO(j, "min_width_ratio_between_bgr", c.min_width_ratio_between_bgr);
  LOAD_JSON_TO(j, "max_width_ratio_between_bgr", c.max_width_ratio_between_bgr);
  LOAD_JSON_TO(j, "min_height_ratio_between_bgr",
               c.min_height_ratio_between_bgr);
  LOAD_JSON_TO(j, "max_height_ratio_between_bgr",
               c.max_height_ratio_between_bgr);
}

void suanzi::from_json(const json &j, ConfigData &c) {
  LOAD_JSON_TO(j, "user", c.user);
  LOAD_JSON_TO(j, "app", c.app);
  LOAD_JSON_TO(j, "temperature", c.temperature);
  LOAD_JSON_TO(j, "quface", c.quface);

  if (j.contains("cameras")) {
    LOAD_JSON_TO(j.at("cameras"), "normal", c.normal);
    LOAD_JSON_TO(j.at("cameras"), "infrared", c.infrared);
  }

  if (j.contains("pro")) {
    LOAD_JSON_TO(j.at("pro"), "detect_levels", c.detect_levels_);
    LOAD_JSON_TO(j.at("pro"), "extract_levels", c.extract_levels_);
    LOAD_JSON_TO(j.at("pro"), "liveness_levels", c.liveness_levels_);
  }
}

void suanzi::to_json(json &j, const ConfigData &c) {
  SAVE_JSON_TO(j, "user", c.user);
  SAVE_JSON_TO(j, "app", c.app);
  SAVE_JSON_TO(j, "temperature", c.temperature);
  SAVE_JSON_TO(j, "quface", c.quface);

  json cameras;
  SAVE_JSON_TO(cameras, "normal", c.normal);
  SAVE_JSON_TO(cameras, "infrared", c.infrared);
  SAVE_JSON_TO(j, "cameras", cameras);

  json pro;
  SAVE_JSON_TO(pro, "detect_levels", c.detect_levels_);
  SAVE_JSON_TO(pro, "extract_levels", c.extract_levels_);
  SAVE_JSON_TO(pro, "liveness_levels", c.liveness_levels_);
  SAVE_JSON_TO(j, "pro", pro);
}

Config Config::instance_;

Config *Config::get_instance() { return &instance_; }

void Config::to_json(json &j) { ::to_json(j, instance_.cfg_data_); }

void Config::load_defaults(ConfigData &c) {
  c.app = {
      .recognize_tip_top_percent = 78,
      .server_port = 8010,
      .server_host = "127.0.0.1",
      .image_store_path = APP_DIR_PREFIX "/var/db/upload/",
      .person_service_base_url = "http://127.0.0.1",
      .show_infrared_window = false,
      .infrared_window_percent = 25,
      .show_isp_info_window = ISPInfoWindowNONE,
      .boot_image_path = "boot.jpg",
      .screensaver_image_path = "background.jpg",
      .has_touch_screen = false,
  };

  c.temperature = {
      .temperature_area_debug = false,
      .detect_area_x = 0.15,
      .detect_area_y = 0.05,
      .detect_area_width = 0.7,
      .detect_area_height = 0.9,
      .min_face_width = 0.48 * 0.35,
      .min_face_height = 0.68 * 0.35,
      .temperature_area_x = 0.3125,
      .temperature_area_y = 0,
      .temperature_area_width = 0.5625,
      .temperature_area_height = 1,
      .temperature_area_radius = 6.0 / 16,
      .min_size = 1,
      .manufacturer = -1,
      .temperature_type = 0,
      .sensor_rotation = TemperatureRotation::None,
  };

  c.user = {
      .lang = "zh-CN",
      .blacklist_policy = "alarm",
      .liveness_policy = "alarm",
      .detect_level = "medium",
      .extract_level = "medium",
      .liveness_level = "medium",
      .duplication_interval = 5,
      .duplication_limit = 3,
      .relay_switch_cond = 3,
      .relay_switch_mode = RelaySwitchMode::AllPass,
      .relay_default_state = RelayState::Low,
      .relay_restore_time = 10,
      .enable_temperature = false,
      .enable_anti_spoofing = false,
      .temperature_bias = 0,
      .temperature_finetune = 0,
      .temperature_var = 90,
      .temperature_max = 37.3,
      .temperature_min = 35.0,
      .enable_audio = true,
      .enable_record_audio = true,
      .enable_temperature_audio = true,
      .enable_mask_audio = true,
      .enable_distance_audio = true,
      .enable_pass_audio = true,
      .enable_led = true,
      .enable_screensaver = true,
      .enable_co2 = false,
      .enable_read_card = false,
      .screensaver_timeout = 60,
      .upload_known_person = true,
      .upload_unknown_person = true,
      .upload_hd_snapshot = false,
      .mask_score = 0.7,
      .ir_validate_score = 0.1,
      .bgr_validate_score = 0.1,
      .wdr = false,
  };

  c.quface = {
      .product_key = "",
      .device_name = "",
      .device_secret = "",
      .client_id = "face-service",
      .db_name = APP_DIR_PREFIX "/var/db/quface",
      .model_file_path = "facemodel.bin",
      .license_filename = "license.json",
  };

  c.normal = {
      .index = 1,
      .rotate = 1,
      .flip = 1,
  };

  c.infrared = {
      .index = 0,
      .rotate = 1,
      .flip = 1,
  };

  c.detect_levels_ = {
      .high =
          {
              .threshold = 0.4f,
              .min_face_size = 40,
              .max_yaw = 10,
              .min_yaw = -10,
              .max_pitch = 10,   // disable max pitch
              .min_pitch = -10,  // disable min pitch
              .min_roll = -10,
              .max_roll = 10,
              .min_tracking_iou = 0.9,
              .min_tracking_number = 3,
          },
      .medium =
          {
              .threshold = 0.4f,
              .min_face_size = 30,
              .max_yaw = 15,
              .min_yaw = -15,
              .max_pitch = 15,   // disable max pitch
              .min_pitch = -15,  // disable min pitch
              .min_roll = -15,
              .max_roll = 15,
              .min_tracking_iou = 0.9,
              .min_tracking_number = 3,
          },
      .low =
          {
              .threshold = 0.4f,
              .min_face_size = 30,
              .max_yaw = 15,
              .min_yaw = -15,
              .max_pitch = 15,   // disable max pitch
              .min_pitch = -15,  // disable min pitch
              .min_roll = -15,
              .max_roll = 15,
              .min_tracking_iou = 0.85,
              .min_tracking_number = 2,
          },
  };

  c.extract_levels_ = {
      .high =
          {
              .history_size = 2,
              .min_recognize_count = 2,
              .min_recognize_score = .8f,
              .min_accumulate_score = 1.6f,
              .max_lost_age = 20,
          },
      .medium =
          {
              .history_size = 1,
              .min_recognize_count = 1,
              .min_recognize_score = .8f,
              .min_accumulate_score = .8f,
              .max_lost_age = 20,
          },
      .low =
          {
              .history_size = 1,
              .min_recognize_count = 1,
              .min_recognize_score = .775f,
              .min_accumulate_score = .775f,
              .max_lost_age = 20,
          },
  };

  c.liveness_levels_ = {
      .high =
          {
              .history_size = 5,
              .min_alive_count = 2,
              .continuous_max_lost_count = 3,
              .min_iou_between_bgr = 0.1,
              .min_width_ratio_between_bgr = .5f,
              .max_width_ratio_between_bgr = 2.f,
              .min_height_ratio_between_bgr = .5f,
              .max_height_ratio_between_bgr = 2.f,
          },
      .medium =
          {
              .history_size = 3,
              .min_alive_count = 1,
              .continuous_max_lost_count = 3,
              .min_iou_between_bgr = 0.1,
              .min_width_ratio_between_bgr = .5f,
              .max_width_ratio_between_bgr = 2.f,
              .min_height_ratio_between_bgr = .5f,
              .max_height_ratio_between_bgr = 2.f,
          },
      .low =
          {
              .history_size = 5,
              .min_alive_count = 1,
              .continuous_max_lost_count = 2,
              .min_iou_between_bgr = 0.1,
              .min_width_ratio_between_bgr = .5f,
              .max_width_ratio_between_bgr = 2.f,
              .min_height_ratio_between_bgr = .5f,
              .max_height_ratio_between_bgr = 2.f,
          },
  };
}

SZ_RETCODE Config::load_from_file(const std::string &config_file,
                                  const std::string &config_override_file) {
  config_file_ = config_file;
  config_override_file_ = config_override_file;

  return reload();
}

SZ_RETCODE Config::read_config(json &cfg) {
  ConfigData cfg_data;
  load_defaults(cfg_data);
  json config(cfg_data);

  std::ifstream ifd(config_file_);
  if (!ifd.is_open()) {
    SZ_LOG_ERROR("Open {} failed, write defaults", config_file_);
  } else {
    SZ_LOG_INFO("Load config from {}", config_file_);
    json file_cfg;
    ifd >> file_cfg;
    config.merge_patch(file_cfg);
  }

  std::ofstream o(config_file_);
  if (!o.is_open()) {
    SZ_LOG_ERROR("Open {} for write failed", config_file_);
    return SZ_RETCODE_FAILED;
  }
  o << config.dump(2);
  cfg = config;

  SZ_LOG_INFO("Config updated to {}", config_file_);

  return SZ_RETCODE_OK;
}

SZ_RETCODE Config::read_override_config(json &cfg) {
  std::ifstream ifd(config_override_file_);
  if (!ifd.is_open()) {
    SZ_LOG_ERROR("Open {} failed", config_override_file_);
    return SZ_RETCODE_OK;
  }

  SZ_LOG_INFO("Override config from {}", config_override_file_);
  ifd >> cfg;
  return SZ_RETCODE_OK;
}

SZ_RETCODE Config::write_override_config(const json &cfg) {
  std::ofstream o(config_override_file_);
  if (!o.is_open()) {
    SZ_LOG_WARN("Open {} failed, can't save", config_override_file_);
    return SZ_RETCODE_FAILED;
  }

  o << cfg.dump(2);
  return SZ_RETCODE_OK;
}

SZ_RETCODE Config::reload() {
  {
    std::unique_lock<std::mutex> lock(cfg_mutex_);

    SZ_LOG_INFO("Load from files ...");
    json config;
    SZ_RETCODE ret = read_config(config);
    if (ret != SZ_RETCODE_OK) {
      return ret;
    }

    try {
      json config_patch;
      ret = read_override_config(config_patch);
      if (ret != SZ_RETCODE_OK) {
        return ret;
      }

      if (config_patch.is_array()) {
        bool patch_updated = false;
        for (auto it = config_patch.begin(); it < config_patch.end(); it++) {
          std::string path;
          it->at("path").get_to(path);
          if (!config.contains(json::json_pointer(path))) {
            config_patch.erase(it);
            SZ_LOG_WARN("Config path {} not exist, erased", path);
            patch_updated = true;
          }
        }
        if (patch_updated) {
          ret = write_override_config(config_patch);
          if (ret != SZ_RETCODE_OK) {
            return ret;
          }
        }
        config = config.patch(config_patch);
      }

      config.get_to(cfg_data_);
    } catch (std::exception &exc) {
      SZ_LOG_ERROR("Load error, will using defaults: {}", exc.what());
      config.get_to(cfg_data_);
      dispatch("reload");
      return SZ_RETCODE_OK;
    }
  }

  dispatch("reload");

  return SZ_RETCODE_OK;
}

SZ_RETCODE Config::save_diff(const json &target_patch) {
  try {
    json source;
    SZ_RETCODE ret = read_config(source);
    if (ret != SZ_RETCODE_OK) {
      return ret;
    }

    json target(cfg_data_);
    target.merge_patch(target_patch);

    json diff = json::diff(source, target);

    ret = write_override_config(diff);
    if (ret != SZ_RETCODE_OK) {
      return ret;
    }
  } catch (std::exception &exc) {
    SZ_LOG_ERROR("Save diff error: {}", exc.what());
    return SZ_RETCODE_FAILED;
  }

  return SZ_RETCODE_OK;
}

SZ_RETCODE Config::reset() {
  SZ_LOG_INFO("Clear everything in {} ...", config_override_file_);
  std::ofstream o(config_override_file_);
  if (!o.is_open()) {
    SZ_LOG_WARN("Open {} failed, can't save", config_override_file_);
    return SZ_RETCODE_FAILED;
  }

  o << "{}";
  return reload();
}

bool Config::load_screen_type(LCDScreenType &lcd_screen_type) {
  std::string conf_filename = "/userdata/user.conf";
  std::ifstream conf(conf_filename);
  if (!conf.is_open()) {
    SZ_LOG_ERROR("Can't open {}", conf_filename);
    return false;
  }

  std::string line;
  std::regex reg("^LCD=(\\d+).+", std::regex_constants::ECMAScript |
                                      std::regex_constants::icase);
  std::smatch matches;
  while (std::getline(conf, line)) {
    if (std::regex_match(line, matches, reg)) {
      if (matches.size() == 2) {
        std::ssub_match base_sub_match = matches[1];
        std::string lcd_type = base_sub_match.str();
        // 0: mipi-7inch-1024x600(default)
        // 1: mipi-8inch-800x1280
        // 2: mipi-10inch-800x1280
        // 3: rh-9881-8inch-800x1280
        // 4: mipi-5inch-480x854
        // 5: LX_ICN9700_5INCH_480x854
        if (lcd_type == "1") {
          lcd_screen_type = RH_8080B1_8INCH_800X1280;
          SZ_LOG_INFO("Load screen type RH_8080B1_8INCH_800X1280");
          return true;
        } else if (lcd_type == "3") {
          lcd_screen_type = RH_9881_8INCH_800X1280;
          SZ_LOG_INFO("Load screen type RH_9881_8INCH_800X1280");
          return true;
        } else if (lcd_type == "4") {
          lcd_screen_type = RH_ST7701S_MIPI_5INCH_480X854;
          SZ_LOG_INFO("Load screen type RH_ST7701S_MIPI_5INCH_480X854");
          return true;
        } else if (lcd_type == "5") {
          lcd_screen_type = LX_ICN9700_5INCH_480x854;
          SZ_LOG_INFO("Load screen type LX_ICN9700_5INCH_480x854");
          return true;
        } else {
          SZ_LOG_ERROR("lcd type unknown {}", lcd_type);
        }
        return false;
      }
    }
  }

  return false;
}

bool Config::load_sensor_type(SensorType &sensor0_type,
                              SensorType &sensor1_type) {
  std::string conf_filename = "/userdata/user.conf";
  std::ifstream conf(conf_filename);
  if (!conf.is_open()) {
    SZ_LOG_ERROR("Can't open {}", conf_filename);
    return false;
  }

  std::string line;
  std::regex reg("^SNS(\\d)_TYPE=(\\d+).+", std::regex_constants::ECMAScript |
                                                std::regex_constants::icase);
  std::smatch matches;
  while (std::getline(conf, line)) {
    if (std::regex_match(line, matches, reg)) {
      if (matches.size() == 3) {
        std::string sensor_id = matches[1].str();
        std::string sensor_type = matches[2].str();

        /**
         * SONY_IMX327_2L_MIPI_2M_30FPS_12BIT = 4
         * SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1 = 6
         * C2395_2L_MIPI_2M_25FPS_10BIT = 5
         * C2395_2L_MIPI_2M_25FPS_10BIT_WDR2TO1 = 7
         */

        SensorType type;
        if (sensor_type == "4") {
          type = SONY_IMX327_2L_MIPI_2M_30FPS_12BIT;
          SZ_LOG_INFO("Load sensor type SONY_IMX327_2L_MIPI_2M_30FPS_12BIT");
        } else if (sensor_type == "5") {
          type = C2395_2L_MIPI_2M_25FPS_10BIT;
          SZ_LOG_INFO("Load sensor type C2395_2L_MIPI_2M_25FPS_10BIT");
        } else if (sensor_type == "6") {
          type = SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1;
          SZ_LOG_INFO(
              "Load sensor type SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1");
        } else if (sensor_type == "7") {
          type = C2395_2L_MIPI_2M_25FPS_10BIT_WDR2TO1;
          SZ_LOG_INFO("Load sensor type C2395_2L_MIPI_2M_25FPS_10BIT_WDR2TO1");
        } else {
          SZ_LOG_ERROR("sensor unknown type {}", sensor_type);
          return false;
        }

        if (sensor_id == "0") {
          sensor0_type = type;
        } else if (sensor_id == "1") {
          sensor1_type = type;
        } else {
          SZ_LOG_ERROR("sensor unknown id {}", sensor_id);
          return false;
        }
      }
    }
  }

  return true;
}

bool Config::load_vo_rotation(ROTATION_E &rotation) {
  rotation = ROTATION_E::ROTATION_0;
  std::string conf_filename = "/userdata/user.conf";
  std::ifstream conf(conf_filename);
  if (!conf.is_open()) {
    SZ_LOG_ERROR("Can't open {}", conf_filename);
    return false;
  }

  std::string line;
  std::regex reg("^HIFB_ROTATE=(\\d+)$", std::regex_constants::ECMAScript |
                                             std::regex_constants::icase);
  std::smatch matches;
  while (std::getline(conf, line)) {
    if (std::regex_match(line, matches, reg)) {
      if (matches.size() == 2) {
        std::ssub_match base_sub_match = matches[1];
        std::string hifb_rotation_value = base_sub_match.str();

        SZ_LOG_INFO("Load HIFB_ROTATE {}", hifb_rotation_value);
        if (hifb_rotation_value == "0") {
          rotation = ROTATION_E::ROTATION_0;
        } else if (hifb_rotation_value == "90") {
        } else if (hifb_rotation_value == "180") {
          rotation = ROTATION_E::ROTATION_180;
        } else if (hifb_rotation_value == "270") {
        } else {
          return false;
        }
      }
    }
  }
  return true;
}

bool Config::read_audio_volume(int &volume_percent) {
  volume_percent = 100;
  std::ifstream volume_cfg_in("/etc/audio-volume");
  if (!volume_cfg_in.is_open()) {
    std::ofstream volume_cfg_out("/etc/audio-volume");
    if (!volume_cfg_out.is_open()) {
      SZ_LOG_ERROR("write volume failed");
      return false;
    }
    volume_cfg_out << volume_percent;
  } else {
    volume_cfg_in >> volume_percent;
  }
  return true;
}

bool Config::write_audio_volume(int volume_percent) {
  std::ofstream volume_cfg_out("/etc/audio-volume");
  if (!volume_cfg_out.is_open()) {
    SZ_LOG_ERROR("write volume failed");
    return false;
  }
  volume_cfg_out << volume_percent;

  return true;
}

bool Config::has_temperature_device() {
  return Config::get_temperature().manufacturer > 0;
}

bool Config::has_read_card_device() {
  return Config::get_user().enable_read_card;
}

bool Config::display_temperature() {
  return Config::get_user().enable_temperature &&
         Config::get_temperature().manufacturer > 0;
}

void Config::set_temperature_finetune(float diff) {
  std::unique_lock<std::mutex> lock(instance_.cfg_mutex_);
  instance_.cfg_data_.user.temperature_finetune += diff;
  if (instance_.cfg_data_.user.temperature_finetune > 2)
    instance_.cfg_data_.user.temperature_finetune = 2;
  if (instance_.cfg_data_.user.temperature_finetune < -2)
    instance_.cfg_data_.user.temperature_finetune = -2;
}

float Config::get_temperature_bias() {
  std::unique_lock<std::mutex> lock(instance_.cfg_mutex_);
  return instance_.cfg_data_.user.temperature_finetune +
         instance_.cfg_data_.user.temperature_bias;
}

const ConfigData &Config::get_all() {
  std::unique_lock<std::mutex> lock(instance_.cfg_mutex_);
  return instance_.cfg_data_;
}

const UserConfig &Config::get_user() {
  std::unique_lock<std::mutex> lock(instance_.cfg_mutex_);
  return instance_.cfg_data_.user;
}

std::string Config::get_user_lang() {
  std::unique_lock<std::mutex> lock(instance_.cfg_mutex_);
  std::string lang = instance_.cfg_data_.user.lang;
  if (lang.find("en") == 0) {
    lang = "en";
  }
  if (lang.size() > 2 && lang[2] == '_') {
    lang[2] = '-';
  }
  return lang;
}

const TemperatureConfig &Config::get_temperature() {
  std::unique_lock<std::mutex> lock(instance_.cfg_mutex_);
  return instance_.cfg_data_.temperature;
}

const AppConfig &Config::get_app() {
  std::unique_lock<std::mutex> lock(instance_.cfg_mutex_);
  return instance_.cfg_data_.app;
}

const QufaceConfig &Config::get_quface() {
  std::unique_lock<std::mutex> lock(instance_.cfg_mutex_);
  return instance_.cfg_data_.quface;
}

const CameraConfig &Config::get_camera(io::CameraType tp) {
  std::unique_lock<std::mutex> lock(instance_.cfg_mutex_);
  if (tp == io::CAMERA_BGR)
    return instance_.cfg_data_.normal;
  else
    return instance_.cfg_data_.infrared;
}

const DetectConfig &Config::get_detect() {
  std::unique_lock<std::mutex> lock(instance_.cfg_mutex_);
  auto &i = instance_;
  return i.cfg_data_.detect_levels_.get(i.cfg_data_.user.detect_level);
}

const ExtractConfig &Config::get_extract() {
  std::unique_lock<std::mutex> lock(instance_.cfg_mutex_);
  auto &i = instance_;
  return i.cfg_data_.extract_levels_.get(i.cfg_data_.user.extract_level);
}

const LivenessConfig &Config::get_liveness() {
  std::unique_lock<std::mutex> lock(instance_.cfg_mutex_);
  auto &i = instance_;
  return i.cfg_data_.liveness_levels_.get(i.cfg_data_.user.liveness_level);
}

bool Config::enable_anti_spoofing() {
  std::unique_lock<std::mutex> lock(instance_.cfg_mutex_);
  return instance_.cfg_data_.user.enable_anti_spoofing;
}

bool Config::has_touch_screen() {
  std::unique_lock<std::mutex> lock(instance_.cfg_mutex_);
  return instance_.cfg_data_.app.has_touch_screen;
}

bool Config::read_image(const std::string &image, const std::string &fallback,
                        std::vector<SZ_BYTE> &data) {
  QFile file(image.c_str());
  if (!file.exists()) {
    file.setFileName(fallback.c_str());
  }
  if (!file.open(QIODevice::ReadOnly)) {
    return false;
  }
  auto content = file.readAll();
  data.assign(content.begin(), content.end());
  return true;
}

bool Config::read_boot_background(std::vector<SZ_BYTE> &data) {
  std::string filename = Config().get_app().boot_image_path;
  return read_image(filename, ":asserts/boot.jpg", data);
}

bool Config::read_screen_saver_background(std::vector<SZ_BYTE> &data) {
  std::string filename = Config().get_app().screensaver_image_path;
  return read_image(filename, ":asserts/background.jpg", data);
}
