#include "detect_task.hpp"

#include <QRect>
#include <QThread>
#include <chrono>
#include <ctime>
#include <iostream>
#include <string>

#include "quface/common.hpp"
#include "quface/face.hpp"

using namespace suanzi;

DetectTask::DetectTask(FaceDetectorPtr detector,
                       FacePoseEstimatorPtr pose_estimator, QThread *thread,
                       QObject *parent)
    : face_detector_(detector), pose_estimator_(pose_estimator) {
  // Initialize PINGPANG buffer
  Size size_bgr_1 = VPSS_CH_SIZES_BGR[1];
  Size size_bgr_2 = VPSS_CH_SIZES_BGR[2];
  if (CH_ROTATES_BGR[1]) {
    size_bgr_1.height = VPSS_CH_SIZES_BGR[1].width;
    size_bgr_1.width = VPSS_CH_SIZES_BGR[1].height;
  }
  if (CH_ROTATES_BGR[2]) {
    size_bgr_2.height = VPSS_CH_SIZES_BGR[2].width;
    size_bgr_2.width = VPSS_CH_SIZES_BGR[2].height;
  }

  Size size_nir_1 = VPSS_CH_SIZES_NIR[1];
  Size size_nir_2 = VPSS_CH_SIZES_NIR[2];
  if (CH_ROTATES_NIR[1]) {
    size_nir_1.height = VPSS_CH_SIZES_NIR[1].width;
    size_nir_1.width = VPSS_CH_SIZES_NIR[1].height;
  }
  if (CH_ROTATES_NIR[2]) {
    size_nir_2.height = VPSS_CH_SIZES_NIR[2].width;
    size_nir_2.width = VPSS_CH_SIZES_NIR[2].height;
  }

  buffer_ping_ =
      new DetectionData(size_bgr_1, size_bgr_2, size_nir_1, size_nir_2);
  buffer_pang_ =
      new DetectionData(size_bgr_1, size_bgr_2, size_nir_1, size_nir_2);
  pingpang_buffer_ =
      new PingPangBuffer<DetectionData>(buffer_ping_, buffer_pang_);

  // Create thread
  if (thread == nullptr) {
    static QThread new_thread;
    moveToThread(&new_thread);
    new_thread.start();
  } else {
    moveToThread(thread);
    thread->start();
  }

  rx_finished_ = true;
}

DetectTask::~DetectTask() {
  if (buffer_ping_) delete buffer_ping_;
  if (buffer_pang_) delete buffer_pang_;
  if (pingpang_buffer_) delete pingpang_buffer_;
}

void DetectTask::rx_frame(PingPangBuffer<ImagePackage> *buffer) {
  auto cfg = Config::get_detect();

  buffer->switch_buffer();
  ImagePackage *input = buffer->get_pang();
  DetectionData *output = pingpang_buffer_->get_ping();
  input->copy_to(*output);

  output->bgr_face_detected_ =
      detect_and_select(input->img_bgr_small, output->bgr_detection_, true);
  emit tx_bgr_display(output->bgr_detection_, !output->bgr_face_detected_,
                      true);

  if (Config::enable_anti_spoofing()) {
    output->nir_face_detected_ =
        detect_and_select(input->img_nir_small, output->nir_detection_, false);
    emit tx_nir_display(output->nir_detection_, !output->nir_face_detected_,
                        false);
  } else
    output->nir_face_detected_ = false;

  if (rx_finished_) {
    rx_finished_ = false;
    emit tx_frame(pingpang_buffer_);
  } else {
    QThread::usleep(10);
  }

  emit tx_finish();
}

void DetectTask::rx_finish() { rx_finished_ = true; }

bool DetectTask::detect_and_select(const MmzImage *image,
                                   DetectionRatio &detection,
                                   bool is_bgr) {
  auto cfg = Config::get_detect();

  // detect faces: 256x256  7ms
  static std::vector<suanzi::FaceDetection> detections;
  suanzi::FacePose pose;
  detections.clear();

  int min_face_size = cfg.min_face_size;
  if (!is_bgr) min_face_size *= 0.8;
  SZ_RETCODE ret =
      face_detector_->detect((const SVP_IMAGE_S *)image->pImplData, detections,
                             cfg.threshold, min_face_size);

  if (ret != SZ_RETCODE_OK) {
    SZ_LOG_ERROR("Detect error ret={}", ret);
    return false;
  }
  if (detections.size() == 0) return false;

  // select largest face
  int max_id = 0;
  float max_area = detections[0].bbox.width * detections[0].bbox.height;
  for (int i = 1; i < detections.size(); i++) {
    float area = detections[i].bbox.width * detections[i].bbox.height;
    if (area > max_area) {
      max_id = i;
      max_area = area;
    }
  }

  ret = pose_estimator_->estimate((const SVP_IMAGE_S *)image->pImplData,
                                  detections[max_id], pose);
  if (ret != SZ_RETCODE_OK) {
    // SZ_LOG_ERROR("Pose estimating error. Low quality", ret);
    return false;
  }

  // return ratio of bbox
  auto rect = detections[max_id].bbox;
  detection.x = rect.x * 1.0 / image->width;
  detection.y = rect.y * 1.0 / image->height;
  detection.width = rect.width * 1.0 / image->width;
  detection.height = rect.height * 0.8 / image->height;  // remove neck

  // return landmarks
  for (int i = 0; i < SZ_LANDMARK_NUM; i++) {
    detection.landmark[i][0] =
        pose.landmarks.point[i].x / image->width;
    detection.landmark[i][1] =
        pose.landmarks.point[i].y / image->height;
  }

  // return head pose
  detection.yaw = pose.yaw;
  detection.pitch = pose.pitch;
  detection.roll = pose.roll;

  return true;
}
