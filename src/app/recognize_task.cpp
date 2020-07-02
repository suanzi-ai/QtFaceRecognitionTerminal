#include "recognize_task.hpp"

#include <QThread>

using namespace suanzi;

RecognzieTask::RecognzieTask(QObject *parent) {
  static QThread thread;
  moveToThread(&thread);
  thread.start();
}

RecognzieTask::~RecognzieTask() {}

void RecognzieTask::rx_frame(PingPangBuffer<MmzImage> *buffer,
                            DetectionFloat detection) {
  MmzImage *pPang = buffer->get_pang();
  // printf("rx1 threadId=%x   %x\n", QThread::currentThreadId(), pPang);
  QThread::msleep(100);

  // if condition
  buffer->switch_buffer();

  static bool bInit = false;
  if (!bInit) {
    static Person person;
    person.name = "hahaha";
    emit tx_result(person);
    bInit = true;
  }
}
