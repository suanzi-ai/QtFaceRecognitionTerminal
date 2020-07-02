#include "person.hpp"

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
}
