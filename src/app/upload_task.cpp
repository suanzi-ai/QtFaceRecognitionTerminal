#include "upload_task.hpp"

#include <QThread>
#include <quface-io/engine.hpp>

using namespace suanzi;
using namespace suanzi::io;

UploadTask::UploadTask(PersonService::ptr person_service, QThread *thread,
                       QObject *parent)
    : person_service_(person_service) {
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

    SZ_RETCODE bgr_encode_result = engine->encode_jpeg(
        bgr_image_buffer, person.bgr_face_snapshot.data,
        person.bgr_face_snapshot.cols, person.bgr_face_snapshot.rows);

    SZ_RETCODE nir_encode_result = engine->encode_jpeg(
        nir_image_buffer, person.nir_face_snapshot.data,
        person.nir_face_snapshot.cols, person.nir_face_snapshot.rows);
    if (nir_encode_result != SZ_RETCODE_FAILED) {
      SZ_LOG_ERROR("Encode nir jpg failed");
    }

    if (bgr_encode_result == SZ_RETCODE_OK)
      person_service_->report_face_record(person.id, bgr_image_buffer,
                                          nir_image_buffer, person.status,
                                          person.temperature);
    else
      SZ_LOG_ERROR("Encode bgr jpg failed");
  }
}
