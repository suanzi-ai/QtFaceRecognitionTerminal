#pragma once

#include <httplib.h>

#include <QMetaType>
#include <nlohmann/json.hpp>
#include <opencv2/opencv.hpp>
#include <quface/common.hpp>
#include <quface/logger.hpp>

namespace suanzi {
using json = nlohmann::json;

struct PersonData {
 public:
  PersonData &operator=(const PersonData &other);

  bool is_status_normal();
  bool is_status_blacklist();
  bool is_temperature_normal();

 public:
  SZ_UINT32 id;
  SZ_FLOAT score;
  SZ_FLOAT temperature;
  SZ_UINT32 age;

  std::string number;
  std::string name;
  std::string gender;
  std::string department;
  std::string mobile;
  std::string status;
  std::string face_url;
  std::string face_path;

  cv::Mat bgr_snapshot;
  cv::Mat nir_snapshot;
  cv::Mat face_snapshot;

  bool is_duplicated;
  bool has_mask;
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

  static PersonService::ptr get_instance();

  SZ_RETCODE get_person(SZ_UINT32 id, PersonData &person);
  SZ_RETCODE get_person(std::string card_no, PersonData &person);

  SZ_RETCODE upload_image(const std::string &type,
                          const std::vector<SZ_UINT8> &image_content,
                          std::string &file_path);
  SZ_RETCODE upload_bgr_image(const std::vector<SZ_UINT8> &image_content,
                              std::string &file_path);
  SZ_RETCODE upload_nir_image(const std::vector<SZ_UINT8> &image_content,
                              std::string &file_path);

  SZ_RETCODE update_person_face_image(
      uint id, const std::vector<SZ_UINT8> &image_content);

  SZ_RETCODE report_face_record(const PersonData &person,
                                const std::vector<SZ_UINT8> &bgr_image_content,
                                const std::vector<SZ_UINT8> &nir_image_content);

  static std::string get_status(PersonStatus s);

  std::string image_store_path_;

 private:
  PersonService(const std::string &scheme_host_port,
                const std::string &image_store_path);

  httplib::Client client_;
};
}  // namespace suanzi

Q_DECLARE_METATYPE(suanzi::PersonData);
