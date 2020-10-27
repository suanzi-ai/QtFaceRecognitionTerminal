#include "upload_task.hpp"

#include <QThread>
#include <quface-io/engine.hpp>

using namespace suanzi;
using namespace suanzi::io;

UploadTask* UploadTask::get_instance() {
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

  if (!if_duplicated) {
    SZ_LOG_DEBUG("upload snapshots");
    auto engine = Engine::instance();

    SZ_RETCODE bgr_encode_result;
    if (person.bgr_face_snapshot.cols > person.bgr_face_snapshot.rows)
      bgr_encode_result = SZ_RETCODE_FAILED;
    else
      bgr_encode_result = engine->encode_jpeg(
          bgr_image_buffer, person.bgr_face_snapshot.data,
          person.bgr_face_snapshot.cols, person.bgr_face_snapshot.rows);

    SZ_RETCODE nir_encode_result;
    if (person.nir_face_snapshot.cols > person.nir_face_snapshot.rows)
      nir_encode_result = SZ_RETCODE_FAILED;
    else
      nir_encode_result = engine->encode_jpeg(
          nir_image_buffer, person.nir_face_snapshot.data,
          person.nir_face_snapshot.cols, person.nir_face_snapshot.rows);
    if (nir_encode_result != SZ_RETCODE_OK)
      SZ_LOG_ERROR("Encode nir jpg failed");

    if (SZ_RETCODE_OK == bgr_encode_result &&
        SZ_RETCODE_OK == nir_encode_result)
      person_service_->report_face_record(person.id, bgr_image_buffer,
                                          nir_image_buffer, person.status,
                                          person.temperature);
    else
      SZ_LOG_ERROR("Encode bgr jpg failed");
  }
}
