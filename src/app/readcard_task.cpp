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

      /*if (SZ_RETCODE_OK ==
      person_service_->get_person(card_no_str.toStdString(), person_data)) {
              qDebug() << "here" << person_data.face_path.c_str();
              emit tx_display(person_data, person_data.is_duplicated);
      }*/
    }
  }
}

#if 0
void ReadCardTask::rx_upload(PersonData person, bool if_duplicated) {
  static std::vector<SZ_UINT8> bgr_image_buffer;
  static std::vector<SZ_UINT8> nir_image_buffer;

  auto cfg = Config::get_user();
  if (!if_duplicated) {
    // whether temperature is enabled but no temperature data
    if (cfg.enable_temperature && person.temperature == 0) return;

    // whether is known person
    if ((person.status == PersonService::get_status(PersonStatus::Normal) ||
         person.status == PersonService::get_status(PersonStatus::Blacklist)) &&
        !cfg.upload_known_person)
      return;

    // whether is unknown person
    if (person.status == PersonService::get_status(PersonStatus::Stranger) &&
        !cfg.upload_unknown_person)
      return;

    SZ_LOG_DEBUG("upload snapshots");
    auto engine = Engine::instance();

    SZ_RETCODE bgr_encode_result;
    if (person.bgr_snapshot.cols > person.bgr_snapshot.rows)
      bgr_encode_result = SZ_RETCODE_FAILED;
    else
      bgr_encode_result = engine->encode_jpeg(
          bgr_image_buffer, person.bgr_snapshot.data, person.bgr_snapshot.cols,
          person.bgr_snapshot.rows);
    if (bgr_encode_result != SZ_RETCODE_OK)
      SZ_LOG_ERROR("Encode bgr jpg failed");

    SZ_RETCODE nir_encode_result;
    if (person.nir_snapshot.cols > person.nir_snapshot.rows)
      nir_encode_result = SZ_RETCODE_FAILED;
    else
      nir_encode_result = engine->encode_jpeg(
          nir_image_buffer, person.nir_snapshot.data, person.nir_snapshot.cols,
          person.nir_snapshot.rows);
    if (nir_encode_result != SZ_RETCODE_OK)
      SZ_LOG_ERROR("Encode nir jpg failed");

    if (SZ_RETCODE_OK == bgr_encode_result &&
        SZ_RETCODE_OK == nir_encode_result)
      person_service_->report_face_record(person, bgr_image_buffer,
                                          nir_image_buffer);
  }
}
#endif
