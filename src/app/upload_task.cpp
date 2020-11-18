#include "upload_task.hpp"

#include <QThread>
#include <quface-io/engine.hpp>

#include "config.hpp"

using namespace suanzi;
using namespace suanzi::io;

UploadTask *UploadTask::get_instance() {
  static UploadTask instance;
  return &instance;
}

UploadTask::UploadTask(QThread *thread, QObject *parent) {
  person_service_ = PersonService::get_instance();

  // Create thread
  if (thread == nullptr) {
    static QThread new_thread;
    moveToThread(&new_thread);
    new_thread.start();
  } else {
    moveToThread(thread);
    thread->start();
  }
}

UploadTask::~UploadTask() {}

void UploadTask::rx_upload(PersonData person, bool if_duplicated) {
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
