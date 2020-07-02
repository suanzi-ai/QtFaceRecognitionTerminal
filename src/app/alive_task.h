#ifndef ALIVE_TASK_H
#define ALIVE_TASK_H

#include <QObject>
#include "pingpang_buffer.h"
#include "image_package.h"
#include "detection_float.h"

namespace suanzi {

class AliveTask : QObject {

    Q_OBJECT
 public:
    AliveTask(QThread *pThread = nullptr, QObject *parent = nullptr);
    ~AliveTask();

 private slots:
    void rxFrame(PingPangBuffer<ImagePackage> *buffer, DetectionFloat detection);

 signals:
    void txFrame(int frame_idx, bool is_live);

 private:

};

}  // namespace suanzi

#endif
