#include "readcard_task.hpp"
#include <QDebug>
#include <QThread>
#include <quface-io/engine.hpp>
#include <quface-io/ic_reader.hpp>
#include "config.hpp"

using namespace suanzi;
using namespace suanzi::io;

ReadCardTask *ReadCardTask::get_instance() {
  static ReadCardTask instance;
  return &instance;
}

ReadCardTask::ReadCardTask(QThread *thread, QObject *parent) {
  person_service_ = PersonService::get_instance();
  start();
}

ReadCardTask::~ReadCardTask() {}

void ReadCardTask::run() {
  auto engine = Engine::instance();
  ICReader::ptr reader = engine->get_ic_reader();
  unsigned char card_no[10];
  int card_no_len = 0;
  PersonData person_data;
  while (1) {
    QThread::usleep(250);
    if (SZ_RETCODE_OK == reader->read_card_no(card_no, card_no_len)) {
      QString card_no_str;
      for (int i = 0; i < card_no_len; i++) {
        card_no_str += QString::number(card_no[i]).sprintf("%02X", card_no[i]);
      }

      if (SZ_RETCODE_OK ==
          person_service_->get_person(card_no_str.toStdString(), person_data)) {
        // qDebug() << "here" << person_data.face_path.c_str();
        emit tx_display(person_data, person_data.is_duplicated);
      }
    }
  }
}
