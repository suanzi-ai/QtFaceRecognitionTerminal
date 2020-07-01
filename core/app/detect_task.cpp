#include "detect_task.h"
#include <QThread>

using namespace suanzi;

DetectTask::DetectTask(QThread *pThread, QObject *parent) {

    if (pThread == nullptr) {
        static QThread thread;
        moveToThread(&thread);
        thread.start();
    } else {
        moveToThread(pThread);
        pThread->start();
    }
}


DetectTask::~DetectTask() {
}


void DetectTask::rxFrame(PingPangBuffer<ImagePackage> *buffer) {
    ImagePackage *pPang = buffer->getPang();
    //printf("rx0 threadId=%x   %x\n", QThread::currentThreadId(), pPang);
    QThread::msleep(100);

    //TODO
    // bgr and nir face detection
    // 2 detection results combine
    // send msgs for next steps


    DetectionFloat  detection_bgr;
    DetectionFloat  detection_nir;
    emit tx_detection_bgr(buffer, detection_bgr);
    emit tx_detection_nir(buffer, detection_nir);

    //printf("tx1 threadId=%x   %x\n", QThread::currentThreadId(), pPing);
}
