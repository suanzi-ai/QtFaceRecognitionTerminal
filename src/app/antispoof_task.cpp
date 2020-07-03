#include "antispoof_task.hpp"

#include <QThread>

using namespace suanzi;

AntispoofTask::AntispoofTask(QThread *thread, QObject *parent) {
  if (thread == nullptr) {
    static QThread new_thread;
    moveToThread(&new_thread);
    new_thread.start();
  } else {
    moveToThread(thread);
    thread->start();
  }
}

AntispoofTask::~AntispoofTask() {}

void AntispoofTask::rx_frame(PingPangBuffer<ImagePackage> *buffer,
                             DetectionFloat detection) {
  ImagePackage *pang = buffer->get_pang();
  QThread::msleep(100);

  bool is_live = true;
  emit tx_frame(pang->frame_idx, is_live);
}
