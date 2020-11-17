#include "config.hpp"

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
  SAVE_JSON_TO(j, "relay_switch_cond", c.relay_switch_cond);
  SAVE_JSON_TO(j, "relay_default_state", c.relay_default_state);
  SAVE_JSON_TO(j, "relay_restore_time", c.relay_restore_time);
  SAVE_JSON_TO(j, "relay_switch_mode", c.relay_switch_mode);
  SAVE_JSON_TO(j, "enable_temperature", c.enable_temperature);
  SAVE_JSON_TO(j, "temperature_bias", c.temperature_bias);
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
  SAVE_JSON_TO(j, "screensaver_timeout", c.screensaver_timeout);
  SAVE_JSON_TO(j, "upload_known_person", c.upload_known_person);
  SAVE_JSON_TO(j, "upload_unknown_person", c.upload_unknown_person);
}

void suanzi::from_json(const json &j, UserConfig &c) {
  LOAD_JSON_TO(j, "lang", c.lang);
  LOAD_JSON_TO(j, "blacklist_policy", c.blacklist_policy);
  LOAD_JSON_TO(j, "liveness_policy", c.liveness_policy);
  LOAD_JSON_TO(j, "detect_level", c.detect_level);
  LOAD_JSON_TO(j, "extract_level", c.extract_level);
  LOAD_JSON_TO(j, "liveness_level", c.liveness_level);
  LOAD_JSON_TO(j, "duplication_interval", c.duplication_interval);
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
  LOAD_JSON_TO(j, "temperature_bias", c.temperature_bias);
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
  LOAD_JSON_TO(j, "screensaver_timeout", c.screensaver_timeout);
  LOAD_JSON_TO(j, "upload_known_person", c.upload_known_person);
  LOAD_JSON_TO(j, "upload_unknown_person", c.upload_unknown_person);
}

void suanzi::to_json(json &j, const AppConfig &c) {
  SAVE_JSON_TO(j, "recognize_tip_top_percent", c.recognize_tip_top_percent);
  SAVE_JSON_TO(j, "server_port", c.server_port);
  SAVE_JSON_TO(j, "server_host", c.server_host);
  SAVE_JSON_TO(j, "image_store_path", c.image_store_path);
  SAVE_JSON_TO(j, "person_service_base_url", c.person_service_base_url);
  SAVE_JSON_TO(j, "enable_anti_spoofing", c.enable_anti_spoofing);
  SAVE_JSON_TO(j, "show_infrared_window", c.show_infrared_window);
  SAVE_JSON_TO(j, "infrared_window_percent", c.infrared_window_percent);
  SAVE_JSON_TO(j, "show_isp_info_window", c.show_isp_info_window);
  SAVE_JSON_TO(j, "boot_image_path", c.boot_image_path);
  SAVE_JSON_TO(j, "screensaver_image_path", c.screensaver_image_path);
}

void suanzi::from_json(const json &j, AppConfig &c) {
  LOAD_JSON_TO(j, "recognize_tip_top_percent", c.recognize_tip_top_percent);
  LOAD_JSON_TO(j, "server_port", c.server_port);
  LOAD_JSON_TO(j, "server_host", c.server_host);
  LOAD_JSON_TO(j, "image_store_path", c.image_store_path);
  LOAD_JSON_TO(j, "person_service_base_url", c.person_service_base_url);
  LOAD_JSON_TO(j, "enable_anti_spoofing", c.enable_anti_spoofing);
  LOAD_JSON_TO(j, "show_infrared_window", c.show_infrared_window);
  LOAD_JSON_TO(j, "infrared_window_percent", c.infrared_window_percent);
  LOAD_JSON_TO(j, "show_isp_info_window", c.show_isp_info_window);
  LOAD_JSON_TO(j, "boot_image_path", c.boot_image_path);
  LOAD_JSON_TO(j, "screensaver_image_path", c.screensaver_image_path);
}

void suanzi::to_json(json &j, const TemperatureConfig &c) {
  SAVE_JSON_TO(j, "device_face_x", c.device_face_x);
  SAVE_JSON_TO(j, "device_face_y", c.device_face_y);
  SAVE_JSON_TO(j, "device_face_height", c.device_face_height);
  SAVE_JSON_TO(j, "device_face_width", c.device_face_width);
  SAVE_JSON_TO(j, "temperature_distance", c.temperature_distance);
  SAVE_JSON_TO(j, "manufacturer", c.manufacturer);
  SAVE_JSON_TO(j, "temperature_delay", c.temperature_delay);
}

void suanzi::from_json(const json &j, TemperatureConfig &c) {
  LOAD_JSON_TO(j, "device_face_x", c.device_face_x);
  LOAD_JSON_TO(j, "device_face_y", c.device_face_y);
  LOAD_JSON_TO(j, "device_face_height", c.device_face_height);
  LOAD_JSON_TO(j, "device_face_width", c.device_face_width);
  LOAD_JSON_TO(j, "temperature_distance", c.temperature_distance);
  LOAD_JSON_TO(j, "manufacturer", c.manufacturer);
  LOAD_JSON_TO(j, "temperature_delay", c.temperature_delay);
}

void suanzi::to_json(json &j, const QufaceConfig &c) {
  SAVE_JSON_TO(j, "product_key", c.product_key);
  SAVE_JSON_TO(j, "device_name", c.device_name);
  SAVE_JSON_TO(j, "device_secret", c.device_secret);
  SAVE_JSON_TO(j, "client_id", c.client_id);
  SAVE_JSON_TO(j, "db_name", c.db_name);
  SAVE_JSON_TO(j, "masked_db_name", c.masked_db_name);
  SAVE_JSON_TO(j, "model_file_path", c.model_file_path);
  SAVE_JSON_TO(j, "license_filename", c.license_filename);
}

void suanzi::from_json(const json &j, QufaceConfig &c) {
  LOAD_JSON_TO(j, "product_key", c.product_key);
  LOAD_JSON_TO(j, "device_name", c.device_name);
  LOAD_JSON_TO(j, "device_secret", c.device_secret);
  LOAD_JSON_TO(j, "client_id", c.client_id);
  LOAD_JSON_TO(j, "db_name", c.db_name);
  LOAD_JSON_TO(j, "masked_db_name", c.masked_db_name);
  LOAD_JSON_TO(j, "model_file_path", c.model_file_path);
  LOAD_JSON_TO(j, "license_filename", c.license_filename);
}

void suanzi::to_json(json &j, const CameraConfig &c) {
  SAVE_JSON_TO(j, "index", c.index);
  SAVE_JSON_TO(j, "rotate", c.rotate);
  SAVE_JSON_TO(j, "flip", c.flip);
  SAVE_JSON_TO(j, "wdr", c.wdr);
  SAVE_JSON_TO(j, "isp", c.isp);
}

void suanzi::from_json(const json &j, CameraConfig &c) {
  LOAD_JSON_TO(j, "index", c.index);
  LOAD_JSON_TO(j, "rotate", c.rotate);
  LOAD_JSON_TO(j, "flip", c.flip);
  LOAD_JSON_TO(j, "wdr", c.wdr);
  LOAD_JSON_TO(j, "isp", c.isp);
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

void suanzi::to_json(json &j, const ISPGlobalConfig &c) {
  SAVE_JSON_TO(j, "adjust_window_size", c.adjust_window_size);
  SAVE_JSON_TO(j, "restore_size", c.restore_size);
}

void suanzi::from_json(const json &j, ISPGlobalConfig &c) {
  LOAD_JSON_TO(j, "adjust_window_size", c.adjust_window_size);
  LOAD_JSON_TO(j, "restore_size", c.restore_size);
}

void suanzi::from_json(const json &j, ConfigData &c) {
  LOAD_JSON_TO(j, "user", c.user);
  LOAD_JSON_TO(j, "app", c.app);
  LOAD_JSON_TO(j, "temperature", c.temperature);
  LOAD_JSON_TO(j, "quface", c.quface);
  LOAD_JSON_TO(j, "isp", c.isp);

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

  SAVE_JSON_TO(j, "isp", c.isp);
}

Config Config::instance_;

Config *Config::get_instance() { return &instance_; }

void Config::load_defaults(ConfigData &c) {
  c.app = {
      .recognize_tip_top_percent = 78,
      .server_port = 8010,
      .server_host = "127.0.0.1",
      .image_store_path = APP_DIR_PREFIX "/var/db/upload/",
      .person_service_base_url = "http://127.0.0.1",
      .enable_anti_spoofing = false,
      .show_infrared_window = false,
      .infrared_window_percent = 25,
      .show_isp_info_window = ISPInfoWindowNONE,
      .boot_image_path = "boot.jpg",
      .screensaver_image_path = "background.jpg",
  };

  c.temperature = {
      .device_face_x = 0.289,
      .device_face_y = 0.18,
      .device_face_height = 0.35,
      .device_face_width = 0.48,
      .temperature_distance = 0.68,
      .temperature_delay = 5,
      .manufacturer = 1,
  };

  c.user = {
      .lang = "zh-CN",
      .blacklist_policy = "alarm",
      .liveness_policy = "alarm",
      .detect_level = "medium",
      .extract_level = "medium",
      .liveness_level = "medium",
      .duplication_interval = 60,
      .relay_switch_cond = 3,
      .relay_switch_mode = RelaySwitchMode::AllPass,
      .relay_default_state = RelayState::Low,
      .relay_restore_time = 10,
      .enable_temperature = false,
      .temperature_bias = 0,
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
      .screensaver_timeout = 60,
      .upload_known_person = true,
      .upload_unknown_person = true,
  };

  c.quface = {
      .product_key = "",
      .device_name = "",
      .device_secret = "",
      .client_id = "face-service",
      .db_name = APP_DIR_PREFIX "/var/db/quface",
      .masked_db_name = APP_DIR_PREFIX "/var/db/quface_masked",
      .model_file_path = "facemodel.bin",
      .license_filename = "license.json",
  };

  c.isp = {
      .adjust_window_size = 10,
      .restore_size = 20,
  };

  c.normal = {
      .index = 1,
      .rotate = 1,
      .flip = 1,
      .wdr = false,
      .isp =
          {
              .stat =
                  {
                      .roi_enable = true,
                      .roi_margin = 2,
                      .roi_weight = 2,
                      .non_roi_weight = 1,
                      .crop_enable = false,
                      .crop_margin = 25,
                  },
              .exposure =
                  {
                      .hist_stat_adjust = true,
                      .speed = 0x60,             // default: 0x40
                      .black_speed_bias = 0x90,  // default: 0x90
                      .tolerance = 0x2,          // default: 0x2
                      .compensation = 0x38,      // default: 0x38
                      .ev_bias = 0x400,          // default: 0x400
                      .ae_strategy_mode =
                          1,  // 0: HIGHLIGHT_PRIOR 1: LOWLIGHT_PRIOR
                      .hist_ratio_slope =
                          0xFF,                 // default: 0x80 range: 0~0xffff
                      .max_hist_offset = 0x10,  // default: 0x10 range: 0~0xff
                      .antiflicker = false,
                      .antiflicker_frequency = 50,
                      .antiflicker_mode = 0,
                  },
              .wdr_exposure =
                  {
                      .exp_radio_type = "manual",
                      .exp_radio = {3096, 64, 64},
                      .exp_radio_max = 0x4000,
                      .exp_radio_min = 0x40,
                      .tolerance = 0xc,
                      .speed = 0x20,
                      .ratio_bias = 0x400,
                  },
              .wb =
                  {
                      .by_pass = false,
                      .op_type = "auto",
                  },
              .saturation =
                  {
                      .op_type = "auto",
                      .manual_saturation = 0x64,
                  },
              .gamma =
                  {
                      .enable = true,
                      .curve_type = 3,
                  },
              .hlc =
                  {
                      .enable = false,
                      .luma_threshold = 240,
                      .luma_target = 10,
                  },
              .drc =
                  {
                      .enable = true,
                      .op_type = "auto",
                  },
          },
  };

  c.infrared = {
      .index = 0,
      .rotate = 1,
      .flip = 1,
      .wdr = false,
      .isp =
          {
              .stat =
                  {
                      .roi_enable = true,
                      .roi_margin = 2,
                      .roi_weight = 2,
                      .non_roi_weight = 1,
                      .crop_enable = false,
                      .crop_margin = 25,
                  },
              .exposure =
                  {
                      .hist_stat_adjust = true,
                      .speed = 0x60,             // default: 0x40
                      .black_speed_bias = 0x90,  // default: 0x90
                      .tolerance = 0x2,          // default: 0x2
                      .compensation = 0x38,      // default: 0x38
                      .ev_bias = 0x400,          // default: 0x400
                      .ae_strategy_mode =
                          1,  // 0: HIGHLIGHT_PRIOR 1: LOWLIGHT_PRIOR
                      .hist_ratio_slope =
                          0xFFF,                // default: 0x80 range: 0~0xffff
                      .max_hist_offset = 0x10,  // default: 0x10 range: 0~0xff
                      .antiflicker = false,
                      .antiflicker_frequency = 50,
                      .antiflicker_mode = 0,
                  },
              .wdr_exposure =
                  {
                      .exp_radio_type = "manual",
                      .exp_radio = {3096, 64, 64},
                      .exp_radio_max = 0x4000,
                      .exp_radio_min = 0x40,
                      .tolerance = 0xc,
                      .speed = 0x20,
                      .ratio_bias = 0x400,
                  },
              .wb =
                  {
                      .by_pass = false,
                      .op_type = "manual",
                  },
              .saturation =
                  {
                      .op_type = "manual",
                      .manual_saturation = 0,
                  },
              .gamma =
                  {
                      .enable = true,
                      .curve_type = 3,
                  },
              .hlc =
                  {
                      .enable = false,
                      .luma_threshold = 240,
                      .luma_target = 10,
                  },
              .drc =
                  {
                      .enable = true,
                      .op_type = "auto",
                  },
          },
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

SZ_RETCODE Config::reload() {
  {
    std::unique_lock<std::mutex> lock(cfg_mutex_);

    SZ_LOG_INFO("Load from files ...");
    try {
      json config;
      SZ_RETCODE ret = read_config(config);
      if (ret != SZ_RETCODE_OK) {
        return ret;
      }

      json config_patch;
      ret = read_override_config(config_patch);
      if (ret != SZ_RETCODE_OK) {
        return ret;
      }

      if (config_patch.is_array()) {
        config = config.patch(config_patch);
      }

      config.get_to(cfg_data_);
    } catch (std::exception &exc) {
      SZ_LOG_ERROR("Load error, will using defaults: {}", exc.what());
      return SZ_RETCODE_FAILED;
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

    std::ofstream o(config_override_file_);
    if (!o.is_open()) {
      SZ_LOG_WARN("Open {} failed, can't save", config_override_file_);
      return SZ_RETCODE_FAILED;
    }

    json diff = json::diff(source, target);

    o << diff.dump(2);
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

const ISPGlobalConfig &Config::get_isp() {
  std::unique_lock<std::mutex> lock(instance_.cfg_mutex_);
  return instance_.cfg_data_.isp;
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
  return instance_.cfg_data_.app.enable_anti_spoofing;
}
