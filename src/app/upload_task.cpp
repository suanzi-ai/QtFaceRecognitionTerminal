#include "upload_task.hpp"

#include <QThread>

#include "venc.hpp"

using namespace suanzi;

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
  static std::vector<SZ_UINT8> image_buffer;

  if (!if_duplicated) {
    SZ_LOG_DEBUG("upload snapshots");
    VEncoder *encoder = VEncoder::get_instance();
    if (encoder->encode(image_buffer, person.face_snapshot.data,
                        person.face_snapshot.cols, person.face_snapshot.rows))
      person_service_->report_face_record(person.id, image_buffer,
                                          person.status);
    else
      SZ_LOG_ERROR("Encode jpg failed");
  }
}
