#pragma once

#include <httplib.h>

#include <QMetaType>
#include <nlohmann/json.hpp>
#include <opencv2/opencv.hpp>
#include <quface/common.hpp>

#include "logger.hpp"

namespace suanzi {
using json = nlohmann::json;

struct PersonData {
  SZ_UINT32 id;
  std::string number;
  std::string name;
  std::string gender;
  SZ_UINT32 age;
  std::string department;
  std::string mobile;
  std::string status;
  float temperature;
  std::string face_url;
  std::string face_path;
  cv::Mat bgr_face_snapshot;
  cv::Mat nir_face_snapshot;
};

void to_json(json &j, const PersonData &p);

void from_json(const json &j, PersonData &p);

enum PersonStatus { Normal, Blacklist, Stranger, Fake, Clear };

class PersonService {
 public:
  typedef std::shared_ptr<PersonService> ptr;

  template <typename... Args>
  static ptr make_shared(Args &&... args) {
    return std::make_shared<PersonService>(std::forward<Args>(args)...);
  }

  PersonService(const std::string &scheme_host_port,
                const std::string &image_store_path)
      : client_(scheme_host_port.c_str()),
        image_store_path_(image_store_path) {}

  std::string get_local_ip();
  std::string get_system_version();

  SZ_RETCODE get_person(SZ_UINT32 id, PersonData &person);

  SZ_RETCODE upload_image(const std::string &type,
                          const std::vector<SZ_UINT8> &image_content,
                          std::string &file_path);
  SZ_RETCODE upload_bgr_image(const std::vector<SZ_UINT8> &image_content,
                              std::string &file_path);
  SZ_RETCODE upload_nir_image(const std::vector<SZ_UINT8> &image_content,
                              std::string &file_path);

  SZ_RETCODE update_person_face_image(
      uint id, const std::vector<SZ_UINT8> &image_content);

  SZ_RETCODE report_face_record(uint person_id,
                                const std::vector<SZ_UINT8> &bgr_image_content,
                                const std::vector<SZ_UINT8> &nir_image_content,
                                const std::string &status,
                                float body_temperature);

  static std::string get_status(PersonStatus s);

 private:
  httplib::Client client_;
  std::string image_store_path_;
};
}  // namespace suanzi

Q_DECLARE_METATYPE(suanzi::PersonData);
