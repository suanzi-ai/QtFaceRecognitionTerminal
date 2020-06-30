#include "recognize_task.h"
#include <QThread>

using namespace suanzi;

RecognzieTask::RecognzieTask(QObject *parent) {
    static QThread thread;
    moveToThread(&thread);
    thread.start();
}


RecognzieTask::~RecognzieTask() {

}


void RecognzieTask::rxFrame(PingPangBuffer<MmzImage> *buffer) {
    MmzImage *pPang = buffer->getPang();
    //printf("rx1 threadId=%x   %x\n", QThread::currentThreadId(), pPang);
    QThread::msleep(100);
    buffer->switchToPing();

    static bool bInit = false;
    if (!bInit) {
        static Person person;
        person.name = "hahaha";
        emit txResult(person);
        bInit = true;
    }
}
