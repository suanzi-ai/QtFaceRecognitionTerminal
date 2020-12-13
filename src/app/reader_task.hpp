#ifndef READER_TASK_H
#define READER_TASK_H

#include <QThread>

#include <quface-io/ic_reader.hpp>

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
  void tx_card_readed(QString card_no);
  void tx_detect_result(bool valid_detect);

 private:
  PersonService::ptr person_service_;
  io::ICReader::ptr reader_;
};

}  // namespace suanzi

#endif
