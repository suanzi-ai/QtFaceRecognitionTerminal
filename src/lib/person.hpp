#ifndef PERSON_H
#define PERSON_H

#include <QMetaType>

namespace suanzi {

class PersonDisplay {
 public:
  PersonDisplay() { to_clear = true; }

  PersonDisplay(std::string name, std::string avatar_path) {
    this->name = name;
    this->avatar_path = avatar_path;
  }

  std::string name;
  std::string avatar_path;
  bool to_clear;
};

}  // namespace suanzi

Q_DECLARE_METATYPE(suanzi::PersonDisplay);

#endif
