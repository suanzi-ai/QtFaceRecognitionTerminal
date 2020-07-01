#include "alive_task.h"
#include <QThread>

using namespace suanzi;

AliveTask::AliveTask(QThread *pThread, QObject *parent) {

    if (pThread == nullptr) {
        static QThread thread;
        moveToThread(&thread);
        thread.start();
    } else {
        moveToThread(pThread);
        pThread->start();
    }
}


AliveTask::~AliveTask() {

}


void AliveTask::rxFrame(PingPangBuffer<ImagePackage> *buffer, DetectionFloat detection) {
    ImagePackage *pPang = buffer->getPang();
    //printf("rx0 threadId=%x   %x\n", QThread::currentThreadId(), pPang);
    QThread::msleep(100);

    bool is_live;
    emit txFrame(pPang->frame_idx, is_live);

    //printf("tx1 threadId=%x   %x\n", QThread::currentThreadId(), pPing);
}
