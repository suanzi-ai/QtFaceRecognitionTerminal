#ifndef PERSON_H
#define PERSON_H

#include <QMetaType>

namespace suanzi {

class PersonDisplay {
 public:
  PersonDisplay() { to_clear = true; }

  PersonDisplay(std::string id, std::string name, std::string avatar_path) {
    this->id = id;
    this->name = name;
    this->avatar_path = avatar_path;
    this->to_clear = false;
  }

  std::string name;
  std::string avatar_path;
  std::string id;

  bool to_clear;
};

}  // namespace suanzi

Q_DECLARE_METATYPE(suanzi::PersonDisplay);

#endif
