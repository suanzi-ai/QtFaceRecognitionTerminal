#ifndef PERSON_H
#define PERSON_H

#include <QMetaType>

class Person {

 public:
    std::string name;
    int age;
    int sex;
};

Q_DECLARE_METATYPE(Person);

#endif
