#ifndef DETECT_TASK_H
#define DETECT_TASK_H

#include <QObject>
#include <QRect>

#include "detection_float.h"
#include "image_package.h"
#include "pingpang_buffer.h"

#include "quface/face.hpp"

namespace suanzi {

class DetectTask : QObject {
  Q_OBJECT
 public:
  DetectTask(QThread *thread = nullptr, QObject *parent = nullptr);
  ~DetectTask();

 private slots:
  void rx_frame(PingPangBuffer<ImagePackage> *buffer);

 signals:
  // for display and recognition
  void tx_detection_bgr(PingPangBuffer<ImagePackage> *buffer,
                        DetectionFloat detection);
  // for face spoofing
  void tx_detection_nir(PingPangBuffer<ImagePackage> *buffer,
                        DetectionFloat detection);

 private:
  int handled_cnt_;

  suanzi::FaceDetector *face_detector_;
};

}  // namespace suanzi

#endif
