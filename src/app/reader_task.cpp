#include "reader_task.hpp"

#include <QDebug>
#include <QThread>

#include <quface-io/engine.hpp>

#include "config.hpp"

using namespace suanzi;
using namespace suanzi::io;

ReaderTask *ReaderTask::get_instance() {
  static ReaderTask instance;
  return &instance;
}

ReaderTask::ReaderTask(QThread *thread, QObject *parent) {
  person_service_ = PersonService::get_instance();
  reader_ = Engine::instance()->get_ic_reader();
  if (reader_ == nullptr) {
    SZ_LOG_ERROR("Get ic reader error");
    return;
  }
  start();
}

ReaderTask::~ReaderTask() {}

void ReaderTask::run() {
  unsigned char card_no[100];
  int card_no_len = 0;

  PersonData person_data;
  while (true) {
    QThread::usleep(250000);
    if (SZ_RETCODE_OK == reader_->read_card_no(card_no, card_no_len)) {
      QString card_no_str;
      for (int i = 0; i < card_no_len; i++)
        card_no_str += QString().sprintf("%02X", card_no[i]);

      emit tx_card_readed(card_no_str);
      emit tx_detect_result(true);
    }
  }
}
