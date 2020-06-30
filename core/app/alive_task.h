#ifndef ALIVE_TASK_H
#define ALIVE_TASK_H

#include <QObject>
#include "pingpangbuffer.h"
#include "image_package.h"

namespace suanzi {

class AliveTask : QObject {

    Q_OBJECT
 public:
    AliveTask(QThread *pThread = nullptr, QObject *parent = nullptr);
    ~AliveTask();

 private slots:
    void rxFrame(PingPangBuffer<ImagePackage> *buffer);

 signals:
    void txFrame(PingPangBuffer<MmzImage> *buffer);

 private:
    PingPangBuffer<MmzImage> *pPingpangBuffer_;
};

}  // namespace suanzi

#endif
