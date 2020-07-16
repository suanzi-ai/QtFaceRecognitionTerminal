#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <fstream>
#include <nlohmann/json.hpp>
#include <quface/common.hpp>
#include <string>

#include "logger.hpp"

namespace suanzi {
using json = nlohmann::json;

typedef struct {
  std::string blacklist_policy;
  std::string detect_level;
  std::string extract_level;
  std::string liveness_level;
  SZ_UINT16 duplication_interval;
} UserConfig;

void to_json(json &j, const UserConfig &c);
void from_json(const json &j, UserConfig &c);

typedef struct {
  SZ_UINT16 server_port;
  std::string server_host;
  std::string image_store_path;
  std::string person_service_base_url;
} AppConfig;

void to_json(json &j, const AppConfig &c);
void from_json(const json &j, AppConfig &c);

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

void to_json(json &j, const CameraConfig &c);
void from_json(const json &j, CameraConfig &c);

typedef struct {
  SZ_FLOAT threshold;
  SZ_UINT32 min_face_size;
  SZ_FLOAT max_yaw;
  SZ_FLOAT min_yaw;
  SZ_FLOAT max_pitch;
  SZ_FLOAT min_pitch;
} DetectConfig;

void to_json(json &j, const DetectConfig &c);
void from_json(const json &j, DetectConfig &c);

typedef struct {
  SZ_INT32 history_size;
  SZ_INT32 min_recognize_count;
  SZ_FLOAT min_recognize_score;
  SZ_FLOAT min_accumulate_score;
  SZ_INT32 max_lost_age;
  SZ_INT32 min_interval_between_same_records;
  SZ_INT32 show_black_list;
} ExtractConfig;

void to_json(json &j, const ExtractConfig &c);
void from_json(const json &j, ExtractConfig &c);

typedef struct {
  bool enable;
  int history_size;
  int min_alive_count;
  int continuous_max_lost_count;
  int max_no_face;
} LivenessConfig;

void to_json(json &j, const LivenessConfig &c);
void from_json(const json &j, LivenessConfig &c);

class Config {
 public:
  typedef std::shared_ptr<Config> ptr;
  static Config::ptr get_instance();

  Config() { load_defaults(); }

  SZ_RETCODE load_from_file(const std::string &config_file,
                            const std::string &config_override_file);
  SZ_RETCODE load_from_json(const json &j);
  SZ_RETCODE reload();
  SZ_RETCODE save();

  static const DetectConfig &get_detect();
  static const ExtractConfig &get_extract();
  static const LivenessConfig &get_liveness();
  static bool is_liveness_enable();

  friend void from_json(const json &j, Config &c);
  friend void to_json(json &j, const Config &c);

 private:
  void load_defaults();

 public:
  UserConfig user;
  AppConfig app;
  QufaceConfig quface;
  CameraConfig normal;
  CameraConfig infrared;

 private:
  static Config instance_;

  std::map<std::string, DetectConfig> detect_levels;
  std::map<std::string, ExtractConfig> extract_levels;
  std::map<std::string, LivenessConfig> liveness_levels;

  DetectConfig default_detect_;
  ExtractConfig default_extract_;
  LivenessConfig default_liveness_;

  std::string config_file_;
  std::string config_override_file_;
};

void from_json(const json &j, Config &c);
void to_json(json &j, const Config &c);

}  // namespace suanzi

#endif
