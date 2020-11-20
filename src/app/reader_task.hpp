#ifndef READER_TASK_H
#define READER_TASK_H

#include <QThread>

#include "person_service.hpp"

namespace suanzi {

class ReaderTask : QThread {
  Q_OBJECT
 public:
  static ReaderTask *get_instance();

 private:
  ReaderTask(QThread *thread = nullptr, QObject *parent = nullptr);
  ~ReaderTask();
  void run() override;

 signals:
  void tx_display(PersonData person, bool if_duplicated);

 private:
  PersonService::ptr person_service_;
};

}  // namespace suanzi

#endif
