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
  std::string face_image;
};

void to_json(json &j, const PersonData &p);

void from_json(const json &j, PersonData &p);

class PersonService {
 public:
  typedef std::shared_ptr<PersonService> ptr;

  PersonService(const std::string &uri, int port) : client_(uri, port) {}

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

 private:
  httplib::Client client_;
};
}  // namespace suanzi
