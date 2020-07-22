#ifndef UPLOAD_TASK_H
#define UPLOAD_TASK_H

#include <QObject>

#include "person_service.hpp"

namespace suanzi {

class UploadTask : QObject {
  Q_OBJECT
 public:
  UploadTask(PersonService::ptr person_service, QThread *thread = nullptr,
             QObject *parent = nullptr);
  ~UploadTask();

 private slots:
  void rx_upload(PersonData person, bool if_duplicated);

 private:
  PersonService::ptr person_service_;
};

}  // namespace suanzi

#endif