#ifndef DETECT_TASK_H
#define DETECT_TASK_H

#include <QObject>
#include "pingpangbuffer.h"

namespace suanzi {

class DetectTask : QObject {

    Q_OBJECT
 public:
    DetectTask(QThread *pThread = nullptr, QObject *parent = nullptr);
    ~DetectTask();

 private slots:
    void rxFrame(PingPangBuffer<MmzImage> *buffer);

 signals:
    void txFrame(PingPangBuffer<MmzImage> *buffer);

 private:
    PingPangBuffer<MmzImage> *pPingpangBuffer_;
};

}  // namespace suanzi

#endif
