#include "alive_task.h"
#include <QThread>

using namespace suanzi;

AliveTask::AliveTask(QThread *pThread, QObject *parent) {

    static MmzImage ping(256, 256, SZ_IMAGETYPE_NV21);
    static MmzImage pang(256, 256, SZ_IMAGETYPE_NV21);
    pPingpangBuffer_ =  new PingPangBuffer<MmzImage>(&ping, &pang);

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


void AliveTask::rxFrame(PingPangBuffer<MmzImage> *buffer) {
    MmzImage *pPang = buffer->getPang();
    printf("rx0 threadId=%x   %x\n", QThread::currentThreadId(), pPang);
    QThread::msleep(100);
    buffer->switchToPing();


    //TODO
    //send detect result
    MmzImage *pPing = pPingpangBuffer_->getPing();

    //TODO
    //fill ping buffer

    pPingpangBuffer_->switchToPang();
    emit txFrame(pPingpangBuffer_);
    printf("tx1 threadId=%x   %x\n", QThread::currentThreadId(), pPing);
}
