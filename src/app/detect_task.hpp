#ifndef DETECT_TASK_H
#define DETECT_TASK_H

#include <QObject>
#include <QRect>
#include <atomic>

#include "config.hpp"
#include "detection_data.hpp"
#include "image_package.hpp"
#include "pingpang_buffer.hpp"
#include "quface_common.hpp"

namespace suanzi {

class DetectTask : QObject {
  Q_OBJECT
 public:
  DetectTask(FaceDetectorPtr detector, FacePoseEstimatorPtr pose_estimator,
             QThread *thread = nullptr, QObject *parent = nullptr);
  ~DetectTask();

  SZ_RETCODE adjust_isp_by_detection(const DetectionData *output);

 private slots:
  void rx_frame(PingPangBuffer<ImagePackage> *buffer);
  void rx_finish();
  void rx_audio_finish();

 signals:
  void tx_finish();

  // for display
  void tx_bgr_display(DetectionRatio detection, bool to_clear, bool valid,
                      bool show_pose);
  void tx_nir_display(DetectionRatio detection, bool to_clear, bool valid,
                      bool show_pose);
  // for audio warning
  void tx_warn_distance();

  // for recognition
  void tx_frame(PingPangBuffer<DetectionData> *buffer);

 private:
  bool detect_and_select(const MmzImage *image, DetectionRatio &detection,
                         bool is_bgr);
  bool check(DetectionRatio detection, bool is_bgr);
  bool is_stable(DetectionRatio detection);

  bool rx_finished_;
  bool audio_finished_;

  FaceDetectorPtr face_detector_;
  FacePoseEstimatorPtr pose_estimator_;

  std::atomic_bool buffer_inited_;
  DetectionData *buffer_ping_, *buffer_pang_;
  PingPangBuffer<DetectionData> *pingpang_buffer_;

  uint detect_count_ = 0;
  uint no_detect_count_ = 0;
};

}  // namespace suanzi

#endif
