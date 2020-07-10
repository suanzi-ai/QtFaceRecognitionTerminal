#ifndef DETECT_TASK_H
#define DETECT_TASK_H

#include <QObject>
#include <QRect>

#include "config.hpp"
#include "detection_float.h"
#include "image_package.h"
#include "pingpang_buffer.h"
#include "quface_common.hpp"

namespace suanzi {

class DetectTask : QObject {
  Q_OBJECT
 public:
  DetectTask(FaceDetectorPtr detector, Config::ptr config,
             QThread *thread = nullptr, QObject *parent = nullptr);
  ~DetectTask();

 private slots:
  void rx_frame(PingPangBuffer<ImagePackage> *buffer);
  void rx_finish();

 signals:
  void tx_finish();

  // for display
  void tx_display(DetectionFloat detection);

  // for recognition
  void tx_recognize(PingPangBuffer<ImagePackage> *buffer,
                    DetectionFloat detection);

 private:
  DetectionFloat select_face(std::vector<suanzi::FaceDetection> &detections,
                             int width, int height);

  bool b_tx_ok_;

  FaceDetectorPtr face_detector_;
  Config::ptr config_;
};

}  // namespace suanzi

#endif
