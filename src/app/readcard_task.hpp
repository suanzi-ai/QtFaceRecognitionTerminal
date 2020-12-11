#ifndef READCARD_TASK_H
#define READCARD_TASK_H

#include <QThread>

#include "person_service.hpp"

namespace suanzi {

class ReadCardTask : QThread {
  Q_OBJECT
 public:
  static ReadCardTask *get_instance();

 private:
  ReadCardTask(QThread *thread = nullptr, QObject *parent = nullptr);
  ~ReadCardTask();
  void run() override;

 signals:
  void tx_display(PersonData person, bool if_duplicated);

 private:
  PersonService::ptr person_service_;
};

}  // namespace suanzi

#endif
