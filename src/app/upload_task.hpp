#ifndef UPLOAD_TASK_H
#define UPLOAD_TASK_H

#include <QObject>

#include "person_service.hpp"

namespace suanzi {

class UploadTask : QObject {
  Q_OBJECT
 public:
  static UploadTask* get_instance();

 private slots:
  void rx_upload(PersonData person, bool if_duplicated);

 private:
  UploadTask(QThread *thread = nullptr, QObject *parent = nullptr);
  ~UploadTask();

  PersonService::ptr person_service_;
};

}  // namespace suanzi

#endif
