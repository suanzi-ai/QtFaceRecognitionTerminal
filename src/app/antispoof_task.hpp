#ifndef ALIVE_TASK_H
#define ALIVE_TASK_H

#include <QObject>

#include "detection_float.h"
#include "image_package.h"
#include "pingpang_buffer.h"

namespace suanzi {

class AntispoofTask : QObject {
  Q_OBJECT
 public:
  AntispoofTask(QThread *thread = nullptr, QObject *parent = nullptr);
  ~AntispoofTask();

 private slots:
  void rx_frame(PingPangBuffer<ImagePackage> *buffer, DetectionFloat detection);

 signals:
  void tx_frame(int frame_idx, bool is_live);

 private:
};

}  // namespace suanzi

#endif
