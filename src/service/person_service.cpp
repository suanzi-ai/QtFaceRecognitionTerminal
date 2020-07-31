#include "person_service.hpp"

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

SZ_RETCODE PersonService::update_person_face_image(
    uint id, const std::vector<SZ_UINT8> &image_content) {
  std::string path = "/api/v1/persons/" + std::to_string(id) + "/faceImage";
  std::string content(image_content.begin(), image_content.end());
  httplib::MultipartFormDataItems items = {
      {"file", content, "avatar.jpg", "images/jpeg"},
  };

  auto res = client_.Post(path.c_str(), items);
  if (res && res->status < 400) {
    // SZ_LOG_DEBUG("Got person body {}", res->body);
    return SZ_RETCODE_OK;
  }
  return SZ_RETCODE_FAILED;
}

SZ_RETCODE PersonService::report_face_record(
    uint person_id, const std::vector<SZ_UINT8> &image_content,
    const std::string &status) {
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
      {"status", status},
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
