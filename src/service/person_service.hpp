#pragma once

#include <QMetaType>

#include <httplib.h>
#include <nlohmann/json.hpp>

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
  std::string face_url;
  std::string face_path;
};

void to_json(json &j, const PersonData &p);

void from_json(const json &j, PersonData &p);

enum PersonStatus {
  Normal,
  Blacklist,
  Stranger,
  Fake,
  Clear
};

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

  SZ_RETCODE get_person(SZ_UINT32 id, PersonData &person) {
    std::string path = "/api/v1/persons/" + std::to_string(id);
    auto res = client_.Get(path.c_str());
    if (res && res->status == 200) {
      // SZ_LOG_DEBUG("Got person body {}", res->body);
      json body = json::parse(res->body);
      body.get_to(person);
      person.face_path = image_store_path_ + person.face_path;
      return SZ_RETCODE_OK;
    }
    return SZ_RETCODE_FAILED;
  }

  SZ_RETCODE report_face_record(uint person_id,
                                const std::vector<SZ_UINT8> &image_content) {
    std::string content(image_content.begin(), image_content.end());
    httplib::MultipartFormDataItems items = {
        {"file", content, "face.jpg", "images/jpeg"},
        {"type", "record", "", ""},
    };

    auto imgRes = client_.Post("/api/v1/images", items);
    if (!imgRes) {
      SZ_LOG_ERROR("Upload image failed, no res");
      return SZ_RETCODE_FAILED;
    }

    if (imgRes->status >= 400) {
      SZ_LOG_ERROR("Upload image failed {}", imgRes->body);
      return SZ_RETCODE_FAILED;
    }

    std::string filePath;
    try {
      json body = json::parse(imgRes->body);
      filePath = body["filePath"];
    } catch (std::exception &exc) {
      SZ_LOG_ERROR("Json parse err {}", exc.what());
      return SZ_RETCODE_FAILED;
    }

    json j = {
        {"personID", person_id},
        {"imagePath", filePath},
    };
    std::string path = "/api/v1/faceRecords";
    auto res = client_.Post(path.c_str(), j.dump(), "application/json");
    if (!res) {
      SZ_LOG_ERROR("Created face record failed, no res");
      return SZ_RETCODE_FAILED;
    }

    if (res->status >= 400) {
      SZ_LOG_ERROR("Create record failed {}", res->body);
      return SZ_RETCODE_FAILED;
    }
    return SZ_RETCODE_OK;
  }

  std::string get_status(PersonStatus s) {
    switch (s) {
      case Normal:
        return "normal";
        break;
      case Blacklist:
        return "blacklist";
        break;
      case Stranger:
        return "stranger";
        break;
      case Fake:
        return "fake";
        break;
      case Clear:
        return "clear";
        break;
      default:
        return "";
        break;
    }
  }

 private:
  httplib::Client2 client_;
  std::string image_store_path_;
};
}  // namespace suanzi


Q_DECLARE_METATYPE(suanzi::PersonData);