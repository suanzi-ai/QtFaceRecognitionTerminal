#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <eventpp/eventdispatcher.h>

#include <fstream>
#include <mutex>
#include <nlohmann/json.hpp>
#include <quface-io/isp_option.hpp>
#include <quface-io/option.hpp>
#include <quface/common.hpp>
#include <quface/logger.hpp>
#include <string>

#define APP_DIR_PREFIX "/user/quface-app"

#define LOAD_JSON_TO(config, key, value) \
  if (config.contains(key)) {            \
    config.at(key).get_to(value);        \
  }

#define SAVE_JSON_TO(config, key, value) config[key] = value;

namespace suanzi {
using json = nlohmann::json;

typedef enum DoorOpenCond {
  Status = 1,
  Temperature = 2,
} DoorOpenCond;

typedef struct {
  std::string lang;
  std::string blacklist_policy;
  std::string liveness_policy;
  std::string detect_level;
  std::string extract_level;
  std::string liveness_level;
  SZ_UINT16 duplication_interval;
  SZ_UINT32 door_open_cond;
  bool disabled_temperature;
  float temperature_max;
  float temperature_min;
  bool enable_audio;
} UserConfig;

void to_json(json &j, const UserConfig &c);
void from_json(const json &j, UserConfig &c);

typedef struct {
  int recognize_tip_top_percent;
  SZ_UINT16 server_port;
  std::string server_host;
  std::string image_store_path;
  std::string person_service_base_url;
  bool enable_anti_spoofing;
  bool show_infrared_window;
  int infrared_window_percent;
  bool show_isp_hist_window;
  std::string boot_image_path;
  std::string screensaver_image_path;
} AppConfig;

void to_json(json &j, const AppConfig &c);
void from_json(const json &j, AppConfig &c);

typedef struct {
  SZ_FLOAT device_face_x;
  SZ_FLOAT device_face_y;
  SZ_FLOAT device_face_height;
  SZ_FLOAT device_face_width;
  SZ_FLOAT temperature_distance;
  SZ_UINT16 device_body_start_angle;
  SZ_UINT16 device_body_open_angle;
  int manufacturer;
} TemperatureConfig;

void to_json(json &j, const TemperatureConfig &c);
void from_json(const json &j, TemperatureConfig &c);

typedef struct {
  std::string product_key;
  std::string device_name;
  std::string device_secret;
  std::string client_id;
  std::string db_name;
  std::string model_file_path;
  std::string license_filename;
} QufaceConfig;

void to_json(json &j, const QufaceConfig &c);
void from_json(const json &j, QufaceConfig &c);

typedef struct {
  SZ_FLOAT threshold;
  SZ_UINT32 min_face_size;
  SZ_FLOAT max_yaw;
  SZ_FLOAT min_yaw;
  SZ_FLOAT max_pitch;
  SZ_FLOAT min_pitch;
  SZ_FLOAT min_roll;
  SZ_FLOAT max_roll;
  SZ_FLOAT min_tracking_iou;
  SZ_UINT32 min_tracking_number;
} DetectConfig;

void to_json(json &j, const DetectConfig &c);
void from_json(const json &j, DetectConfig &c);

typedef struct {
  SZ_INT32 history_size;
  SZ_INT32 min_recognize_count;
  SZ_FLOAT min_recognize_score;
  SZ_FLOAT min_accumulate_score;
  SZ_INT32 max_lost_age;
} ExtractConfig;

void to_json(json &j, const ExtractConfig &c);
void from_json(const json &j, ExtractConfig &c);

typedef struct {
  SZ_INT32 history_size;
  SZ_INT32 min_alive_count;
  SZ_INT32 continuous_max_lost_count;
  SZ_FLOAT min_iou_between_bgr;
  SZ_FLOAT min_width_ratio_between_bgr;
  SZ_FLOAT max_width_ratio_between_bgr;
  SZ_FLOAT min_height_ratio_between_bgr;
  SZ_FLOAT max_height_ratio_between_bgr;
} LivenessConfig;

void to_json(json &j, const LivenessConfig &c);
void from_json(const json &j, LivenessConfig &c);

typedef struct {
  int adjust_window_size;  // 调整间隔，聚焦于人脸
  int restore_size;        // 调整恢复间隔，聚焦于中心
} ISPGlobalConfig;

void to_json(json &j, const ISPGlobalConfig &c);
void from_json(const json &j, ISPGlobalConfig &c);

typedef struct {
  int index;
  int rotate;
  int flip;
  ISPConfig isp;
} CameraConfig;

void to_json(json &j, const CameraConfig &c);
void from_json(const json &j, CameraConfig &c);

template <typename T>
struct Levels {
  T high;
  T medium;
  T low;
  T &get(const std::string &level) {
    if (level == "high") {
      return high;
    } else if (level == "medium") {
      return medium;
    } else if (level == "low") {
      return low;
    }
    return medium;
  }
};

template <typename T>
void to_json(json &j, const Levels<T> &c) {
  SAVE_JSON_TO(j, "high", c.high);
  SAVE_JSON_TO(j, "medium", c.medium);
  SAVE_JSON_TO(j, "low", c.low);
}

template <typename T>
void from_json(const json &j, Levels<T> &c) {
  LOAD_JSON_TO(j, "high", c.high);
  LOAD_JSON_TO(j, "medium", c.medium);
  LOAD_JSON_TO(j, "low", c.low);
}

typedef struct {
  UserConfig user;
  AppConfig app;
  TemperatureConfig temperature;
  QufaceConfig quface;
  CameraConfig normal;
  CameraConfig infrared;
  ISPGlobalConfig isp;
  Levels<DetectConfig> detect_levels_;
  Levels<ExtractConfig> extract_levels_;
  Levels<LivenessConfig> liveness_levels_;
} ConfigData;

void from_json(const json &j, ConfigData &c);
void to_json(json &j, const ConfigData &c);

typedef eventpp::EventDispatcher<std::string, void()> ConfigEventEmitter;

class Config : public ConfigEventEmitter {
 public:
  typedef std::shared_ptr<Config> ptr;
  static Config *get_instance();

  SZ_RETCODE load_from_file(const std::string &config_file,
                            const std::string &config_override_file);
  SZ_RETCODE reload();
  SZ_RETCODE save_diff(const json &target);
  SZ_RETCODE reset();
  static bool load_screen_type(io::LCDScreenType &lcd_screen_type);

  static const ConfigData &get_all();
  static const UserConfig &get_user();
  static const TemperatureConfig &get_temperature();
  static const AppConfig &get_app();
  static const QufaceConfig &get_quface();
  static const CameraConfig &get_camera(io::CameraType tp);
  static const DetectConfig &get_detect();
  static const ExtractConfig &get_extract();
  static const LivenessConfig &get_liveness();
  static const ISPGlobalConfig &get_isp();

  static std::string get_user_lang();
  static bool enable_anti_spoofing();

 private:
  void load_defaults(ConfigData &c);
  SZ_RETCODE read_config(json &cfg);
  SZ_RETCODE read_override_config(json &cfg);

 private:
  mutable std::mutex cfg_mutex_;
  ConfigData cfg_data_;
  static Config instance_;

  std::string config_file_;
  std::string config_override_file_;
};

}  // namespace suanzi

#endif
