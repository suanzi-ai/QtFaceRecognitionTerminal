#ifndef DETECT_TASK_H
#define DETECT_TASK_H

#include <QObject>
#include <QRect>

#include "config.hpp"
#include "detection_data.hpp"
#include "image_package.hpp"
#include "pingpang_buffer.hpp"
#include "quface_common.hpp"

namespace suanzi {

class DetectTask : QObject {
  Q_OBJECT
 public:
  DetectTask(FaceDetectorPtr detector, FacePoseEstimatorPtr pose_estimator, QThread *thread = nullptr,
             QObject *parent = nullptr);
  ~DetectTask();

 private slots:
  void rx_frame(PingPangBuffer<ImagePackage> *buffer);
  void rx_finish();

 signals:
  void tx_finish();

  // for display
  void tx_bgr_display(DetectionRatio detection, bool to_clear);
  void tx_nir_display(DetectionRatio detection, bool to_clear);

  // for recognition
  void tx_frame(PingPangBuffer<DetectionData> *buffer);

 private:
  // nyy
  const Size VPSS_CH_SIZES_BGR[3] = {
      {1920, 1080}, {1080, 704}, {320, 224}};  // larger small
  const Size VPSS_CH_SIZES_NIR[3] = {
      {1920, 1080}, {1080, 704}, {320, 224}};  // larger small
  const int CH_INDEXES_BGR[3] = {0, 1, 2};
  const bool CH_ROTATES_BGR[3] = {false, true, true};
  const int CH_INDEXES_NIR[3] = {0, 1, 2};
  const bool CH_ROTATES_NIR[3] = {false, true, true};

  bool detect_and_select(const MmzImage *image, DetectionRatio &detection);

  bool rx_finished_;

  FaceDetectorPtr face_detector_;
  FacePoseEstimatorPtr pose_estimator_;

  DetectionData *buffer_ping_, *buffer_pang_;
  PingPangBuffer<DetectionData> *pingpang_buffer_;
};

}  // namespace suanzi

#endif
