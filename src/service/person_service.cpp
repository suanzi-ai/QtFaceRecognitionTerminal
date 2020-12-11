#include "person_service.hpp"

#include "config.hpp"

void suanzi::to_json(json &j, const PersonData &p) {
  j = json{
      {"id", p.id},
      {"number", p.number},
      {"name", p.name},
      {"gender", p.gender},
      {"age", p.age},
      {"department", p.department},
      {"mobile", p.mobile},
      {"status", p.status},
      {"faceURL", p.face_url},
      {"facePath", p.face_path},
  };
}

void suanzi::from_json(const json &j, PersonData &p) {
  try {
    j.at("id").get_to(p.id);
    j.at("number").get_to(p.number);
    j.at("name").get_to(p.name);
    j.at("gender").get_to(p.gender);
    j.at("age").get_to(p.age);
    j.at("department").get_to(p.department);
    j.at("mobile").get_to(p.mobile);
    j.at("status").get_to(p.status);

    if (j.contains("faceURL")) {
      j.at("faceURL").get_to(p.face_url);
    }

    if (j.contains("facePath")) {
      j.at("facePath").get_to(p.face_path);
    }
  } catch (std::exception &exc) {
    SZ_LOG_ERROR("Parse json error {}", exc.what());
  }
}

using namespace suanzi;

bool PersonData::is_status_normal() {
  return status == PersonService::get_status(PersonStatus::Normal);
}

bool PersonData::is_status_blacklist() {
  return status == PersonService::get_status(PersonStatus::Blacklist);
}

bool PersonData::is_temperature_normal() {
  auto user = Config::get_user();
  if (!user.enable_temperature) return true;

  return temperature >= user.temperature_min &&
         temperature <= user.temperature_max;
}

PersonService::ptr PersonService::get_instance() {
  static PersonService instance(Config::get_app().person_service_base_url,
                                Config::get_app().image_store_path);
  return PersonService::ptr(&instance);
}

PersonService::PersonService(const std::string &scheme_host_port,
                             const std::string &image_store_path)
    : client_(scheme_host_port.c_str()), image_store_path_(image_store_path) {}

SZ_RETCODE PersonService::get_person(SZ_UINT32 id, PersonData &person) {
  std::string path = "/api/v1/persons/" + std::to_string(id);
  auto res = client_.Get(path.c_str());
  if (res && res->status < 400) {
    // SZ_LOG_DEBUG("Got person body {}", res->body);
    json body = json::parse(res->body);
    body.get_to(person);
    person.face_path = image_store_path_ + person.face_path;
    return SZ_RETCODE_OK;
  }
  return SZ_RETCODE_FAILED;
}

SZ_RETCODE PersonService::get_person(std::string card_no, PersonData &person) {
  std::string path = "/api/v1/persons-by-number/" + card_no;
  auto res = client_.Get(path.c_str());
  if (res && res->status < 400) {
    // SZ_LOG_DEBUG("Got person body {}", res->body);
    json body = json::parse(res->body);
    body.get_to(person);
    person.face_path = image_store_path_ + person.face_path;
    return SZ_RETCODE_OK;
  }
  return SZ_RETCODE_FAILED;
}


SZ_RETCODE PersonService::update_person_face_image(
    uint id, const std::vector<SZ_UINT8> &image_content) {
  std::string path = "/api/v1/persons/" + std::to_string(id) + "/faceImage";
  std::string content(image_content.begin(), image_content.end());
  httplib::MultipartFormDataItems items = {
      {"file", content, "avatar.jpg", "images/jpeg"},
  };

  auto res = client_.Post(path.c_str(), items);
  if (res && res->status < 400) {
    return SZ_RETCODE_OK;
  } else {
    SZ_LOG_ERROR("Got person body {}", res->body);
    return SZ_RETCODE_FAILED;
  }
}

SZ_RETCODE PersonService::upload_bgr_image(
    const std::vector<SZ_UINT8> &image_content, std::string &file_path) {
  return upload_image("record", image_content, file_path);
}

SZ_RETCODE PersonService::upload_nir_image(
    const std::vector<SZ_UINT8> &image_content, std::string &file_path) {
  return upload_image("ir-record", image_content, file_path);
}

SZ_RETCODE PersonService::upload_image(
    const std::string &type, const std::vector<SZ_UINT8> &image_content,
    std::string &file_path) {
  std::string content(image_content.begin(), image_content.end());
  httplib::MultipartFormDataItems items = {
      {"file", content, "face.jpg", "images/jpeg"},
      {"type", type, "", ""},
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

  try {
    json body = json::parse(imgRes->body);
    file_path = body["filePath"];
  } catch (std::exception &exc) {
    SZ_LOG_ERROR("Json parse err {}", exc.what());
    return SZ_RETCODE_FAILED;
  }

  return SZ_RETCODE_OK;
}

SZ_RETCODE PersonService::report_face_record(
    const PersonData &person, const std::vector<SZ_UINT8> &bgr_image_content,
    const std::vector<SZ_UINT8> &nir_image_content) {
  std::string bgr_file_path;
  SZ_RETCODE ret = upload_bgr_image(bgr_image_content, bgr_file_path);
  if (ret != SZ_RETCODE_OK) {
    return ret;
  }

  std::string nir_file_path;
  if (!nir_image_content.empty()) {
    ret = upload_nir_image(nir_image_content, nir_file_path);
    if (ret != SZ_RETCODE_OK) {
      return ret;
    }
  }

  json j = {
      {"personID", person.id},
      {"imagePath", bgr_file_path},
      {"irImagePath", nir_file_path},
      {"status", person.status},
      {"temperature", person.temperature},
      {"maskStatus", person.has_mask ? "correct" : "none"},
  };
  if (!Config::get_user().enable_temperature) j.erase("temperature");

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

std::string PersonService::get_status(PersonStatus s) {
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
