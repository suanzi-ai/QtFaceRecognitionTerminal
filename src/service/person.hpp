#pragma once

#include <httplib.h>

#include <nlohmann/json.hpp>
#include <quface/common.hpp>

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

class PersonService {
 public:
  typedef std::shared_ptr<PersonService> ptr;

  PersonService(const std::string &scheme_host_port)
      : client_(scheme_host_port.c_str()) {}

  SZ_RETCODE get_person(uint id, PersonData &person) {
    std::string path = "/api/v1/persons/" + std::to_string(id);
    auto res = client_.Get(path.c_str());
    if (res && res->status == 200) {
      json body(res->body);
      body.get_to(person);
      return SZ_RETCODE_OK;
    }
    return SZ_RETCODE_FAILED;
  }

  SZ_RETCODE report_face_record(uint person_id, const std::string &status,
                                const std::string &image_content) {
    httplib::MultipartFormDataItems items = {
        {"file", image_content, "face.jpg", "images/jpeg"},
        {"type", "record", "", ""},
    };

    std::string filePath;
    auto imgRes = client_.Post("/api/v1/images", items);
    if (imgRes && imgRes->status == 200) {
      json body(imgRes->body);
      filePath = body["filePath"];
      return SZ_RETCODE_OK;
    }

    json j = {
        {"personID", person_id},
        {"status", status},
        {"facePath", filePath},
    };
    std::string path = "/api/v1/faceRecords";
    auto res = client_.Post(path.c_str(), j.dump(), "application/json");
    if (res && res->status == 200) {
      return SZ_RETCODE_OK;
    }
    return SZ_RETCODE_FAILED;
  }

 private:
  httplib::Client2 client_;
};
}  // namespace suanzi
