#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <fstream>
#include <mutex>
#include <string>

#include <eventpp/eventdispatcher.h>
#include <nlohmann/json.hpp>

#include <quface-io/isp_option.hpp>
#include <quface-io/option.hpp>
#include <quface/common.hpp>
#include <quface/logger.hpp>

#define APP_DIR_PREFIX "/user/quface-app"

#define LOAD_JSON_TO(config, key, value) \
  if (config.contains(key)) {            \
    config.at(key).get_to(value);        \
  }

#define SAVE_JSON_TO(config, key, value) config[key] = value;

namespace suanzi {
using json = nlohmann::json;

typedef enum RelaySwitchCond {
  Status = 1,
  Temperature = 2,
  Mask = 4,
} RelaySwitchCond;

typedef enum RelayState {
  Low = 0,
  High = 1,
} RelayState;

typedef enum RelaySwitchMode {
  AllPass = 0,
  AnyNotPass = 1,
} RelaySwitchMode;

typedef struct {
  std::string lang;
  std::string blacklist_policy;
  std::string liveness_policy;
  std::string detect_level;
  std::string extract_level;
  std::string liveness_level;
  SZ_UINT16 duplication_interval;
  SZ_UINT16 duplication_limit;
  SZ_UINT32 relay_switch_cond;
  RelaySwitchMode relay_switch_mode;
  RelayState relay_default_state;
  SZ_UINT16 relay_restore_time;
  bool enable_temperature;
  bool enable_anti_spoofing;
  SZ_FLOAT temperature_bias;
  SZ_FLOAT temperature_finetune;
  SZ_FLOAT temperature_var;
  SZ_FLOAT temperature_max;
  SZ_FLOAT temperature_min;
  bool enable_audio;
  bool enable_record_audio;
  bool enable_temperature_audio;
  bool enable_mask_audio;
  bool enable_distance_audio;
  bool enable_pass_audio;
  bool enable_led;
  bool enable_screensaver;
  bool enable_co2;
  bool enable_read_card;
  SZ_UINT16 screensaver_timeout;
  bool upload_known_person;
  bool upload_unknown_person;
  bool upload_hd_snapshot;
  SZ_FLOAT mask_score;
  SZ_FLOAT ir_validate_score;
  SZ_FLOAT bgr_validate_score;
  bool wdr;
} UserConfig;

void to_json(json &j, const UserConfig &c);
void from_json(const json &j, UserConfig &c);

typedef enum ISPInfoWindowType {
  ISPInfoWindowNONE,
  ISPInfoWindowBGR,
  ISPInfoWindowNIR,
} ISPInfoWindowType;

typedef struct {
  int recognize_tip_top_percent;
  SZ_UINT16 server_port;
  std::string server_host;
  std::string image_store_path;
  std::string person_service_base_url;
  bool show_infrared_window;
  int infrared_window_percent;
  ISPInfoWindowType show_isp_info_window;
  std::string boot_image_path;
  std::string screensaver_image_path;
  bool has_touch_screen;
} AppConfig;

void to_json(json &j, const AppConfig &c);
void from_json(const json &j, AppConfig &c);

typedef enum TemperatureRotation {
  None = 0,
  ROTATION_90 = 90,
  ROTATION_180 = 180,
  ROTATION_270 = 270
} TemperatureRotation;

typedef struct {
  bool temperature_area_debug;

  SZ_FLOAT detect_area_x;
  SZ_FLOAT detect_area_y;
  SZ_FLOAT detect_area_width;
  SZ_FLOAT detect_area_height;

  SZ_FLOAT min_face_width;
  SZ_FLOAT min_face_height;

  SZ_FLOAT temperature_area_x;
  SZ_FLOAT temperature_area_y;
  SZ_FLOAT temperature_area_width;
  SZ_FLOAT temperature_area_height;
  SZ_FLOAT temperature_area_radius;

  int min_size;
  int manufacturer;
  int temperature_type;
  TemperatureRotation sensor_rotation;
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
  int index;
  int rotate;
  int flip;
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
  static void to_json(json &j);

  SZ_RETCODE load_from_file(const std::string &config_file,
                            const std::string &config_override_file);
  SZ_RETCODE reload();
  SZ_RETCODE save_diff(const json &target);
  SZ_RETCODE reset();
  static bool load_screen_type(io::LCDScreenType &lcd_screen_type);
  static bool load_sensor_type(io::SensorType &sensor0_type,
                               io::SensorType &sensor1_type);
  static bool load_vo_rotation(ROTATION_E &rotation);
  static bool write_audio_volume(int volume_percent);
  static bool read_audio_volume(int &volume_percent);

  static bool has_temperature_device();
  static bool has_read_card_device();
  static bool display_temperature();
  static void set_temperature_finetune(float bias);
  static float get_temperature_bias();

  static const ConfigData &get_all();
  static const UserConfig &get_user();
  static const TemperatureConfig &get_temperature();
  static const AppConfig &get_app();
  static const QufaceConfig &get_quface();
  static const CameraConfig &get_camera(io::CameraType tp);
  static const DetectConfig &get_detect();
  static const ExtractConfig &get_extract();
  static const LivenessConfig &get_liveness();

  static std::string get_user_lang();
  static bool enable_anti_spoofing();

  static bool has_touch_screen();

  static bool read_boot_background(std::vector<SZ_BYTE> &data);
  static bool read_screen_saver_background(std::vector<SZ_BYTE> &data);

 private:
  void load_defaults(ConfigData &c);
  SZ_RETCODE read_config(json &cfg);
  SZ_RETCODE read_override_config(json &cfg);
  SZ_RETCODE write_override_config(const json &cfg);

  static bool read_image(const std::string &image, const std::string &fallback,
                         std::vector<SZ_BYTE> &data);

 private:
  mutable std::mutex cfg_mutex_;
  ConfigData cfg_data_;
  static Config instance_;

  std::string config_file_;
  std::string config_override_file_;
};

}  // namespace suanzi

#endif
