#include "detect_task.hpp"

#include <chrono>
#include <ctime>
#include <iostream>
#include <string>

#include <QRect>
#include <QThread>

#include <quface-io/engine.hpp>
#include <quface/common.hpp>
#include <quface/face.hpp>

#include "audio_task.hpp"
#include "config.hpp"
#include "recognize_task.hpp"
#include "temperature_task.hpp"

using namespace suanzi;

DetectTask *DetectTask::get_instance() {
  static DetectTask instance;
  return &instance;
}

DetectTask::DetectTask(QThread *thread, QObject *parent)
    : buffer_inited_(false) {
  auto cfg = Config::get_quface();
  face_detector_ = std::make_shared<FaceDetector>(cfg.model_file_path);
  pose_estimator_ = std::make_shared<FacePoseEstimator>(cfg.model_file_path);

  // Create thread
  if (thread == nullptr) {
    static QThread new_thread;
    moveToThread(&new_thread);
    new_thread.start();
  } else {
    moveToThread(thread);
    thread->start();
  }
}

DetectTask::~DetectTask() {
  if (buffer_ping_) delete buffer_ping_;
  if (buffer_pang_) delete buffer_pang_;
  if (pingpang_buffer_) delete pingpang_buffer_;
}

SZ_RETCODE DetectTask::adjust_isp_by_detection(const DetectionData *output) {
  auto isp_global = Config::get_isp();
  auto bgr_cam = Config::get_camera(CAMERA_BGR);
  auto nir_cam = Config::get_camera(CAMERA_NIR);

  ROICfg bgr_roi_cfg = {0.3, 0.3, 0.4, 0.4};
  ROICfg nir_roi_cfg = {0.3, 0.3, 0.4, 0.4};

  SZ_RETCODE ret;

  auto engine = io::Engine::instance();
  if (detect_count_ % isp_global.adjust_window_size ==
      isp_global.adjust_window_size - 1) {
    if (output->bgr_face_detected_) {
      auto det = output->bgr_detection_;
      bgr_roi_cfg.x = det.x;
      bgr_roi_cfg.y = det.y;
      bgr_roi_cfg.width = det.width;
      bgr_roi_cfg.height = det.height;

      ret = engine->isp_set_roi(CAMERA_BGR, &bgr_roi_cfg, &bgr_cam.isp.stat);
      if (ret != SZ_RETCODE_OK) {
        return ret;
      }
    } else if (output->nir_face_detected_) {
      auto det = output->nir_detection_;
      nir_roi_cfg.x = det.x;
      nir_roi_cfg.y = det.y;
      nir_roi_cfg.width = det.width;
      nir_roi_cfg.height = det.height;

      ret = engine->isp_set_roi(CAMERA_NIR, &nir_roi_cfg, &nir_cam.isp.stat);
      if (ret != SZ_RETCODE_OK) {
        return ret;
      }
    }

    if (output->nir_face_detected_) {
      auto det = output->nir_detection_;
      nir_roi_cfg.x = det.x;
      nir_roi_cfg.y = det.y;
      nir_roi_cfg.width = det.width;
      nir_roi_cfg.height = det.height;

      ret = engine->isp_set_roi(CAMERA_NIR, &nir_roi_cfg, &nir_cam.isp.stat);
      if (ret != SZ_RETCODE_OK) {
        return ret;
      }
    }
  }

  if (no_detect_count_ == isp_global.restore_size) {
    ret = engine->isp_set_roi(CAMERA_BGR, &bgr_roi_cfg, &bgr_cam.isp.stat);
    if (ret != SZ_RETCODE_OK) {
      return ret;
    }

    ret = engine->isp_set_roi(CAMERA_NIR, &nir_roi_cfg, &nir_cam.isp.stat);
    if (ret != SZ_RETCODE_OK) {
      return ret;
    }
  }

  return SZ_RETCODE_OK;
}

void DetectTask::rx_frame(PingPangBuffer<ImagePackage> *buffer) {
  auto cfg = Config::get_detect();

  buffer->switch_buffer();
  ImagePackage *input = buffer->get_pang();

  bool buffer_inited = false;
  if (buffer_inited_.compare_exchange_strong(buffer_inited, true)) {
    buffer_ping_ = new DetectionData(input);
    buffer_pang_ = new DetectionData(input);
    pingpang_buffer_ =
        new PingPangBuffer<DetectionData>(buffer_ping_, buffer_pang_);
  }

  DetectionData *output = pingpang_buffer_->get_ping();
  input->copy_to(*output);

  output->bgr_face_detected_ =
      detect_and_select(input->img_bgr_small, output->bgr_detection_, true);
  if (output->bgr_face_detected_)
    output->bgr_face_valid_ = check(output->bgr_detection_, true);

  emit tx_bgr_display(output->bgr_detection_, !output->bgr_face_detected_,
                      output->bgr_face_valid_, true);

  if (TemperatureTask::get_instance()->idle())
    emit tx_temperature_target(output->bgr_detection_,
                               !output->bgr_face_detected_);

  output->nir_face_detected_ =
      detect_and_select(input->img_nir_small, output->nir_detection_, false);
  if (output->nir_face_detected_)
    output->nir_face_valid_ = check(output->nir_detection_, false);
  emit tx_nir_display(output->nir_detection_, !output->nir_face_detected_,
                      output->nir_face_valid_, false);

  if (output->bgr_face_detected_ || output->nir_face_detected_) {
    detect_count_++;
    no_detect_count_ = 0;
  } else {
    detect_count_ = 0;
    no_detect_count_++;
  }

  SZ_RETCODE ret = adjust_isp_by_detection(output);
  if (ret != SZ_RETCODE_OK) {
    SZ_LOG_ERROR("Adjust isp failed");
  }

  if (RecognizeTask::idle())
    emit tx_frame(pingpang_buffer_);
  else
    QThread::usleep(10);

  emit tx_finish();
}

bool DetectTask::detect_and_select(const MmzImage *image,
                                   DetectionRatio &detection, bool is_bgr) {
  auto cfg = Config::get_detect();

  // skip broken image
  int width = ((const SVP_IMAGE_S *)image->pImplData)->u32Width;
  int height = ((const SVP_IMAGE_S *)image->pImplData)->u32Height;

  if (width > height) return false;

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

  float prob_threshold = is_bgr ? 0.9 : 0.75;
  ret = pose_estimator_->estimate((const SVP_IMAGE_S *)image->pImplData,
                                  detections[max_id], pose, prob_threshold);
  if (ret != SZ_RETCODE_OK) {
    // SZ_LOG_ERROR("Pose estimating error. Low quality", ret);
    return false;
  }

  // return ratio of bbox
  auto rect = detections[max_id].bbox;
  detection.x = rect.x * 1.0 / image->width;
  detection.y = rect.y * 1.0 / image->height;
  detection.width = rect.width * 1.0 / image->width;
  detection.height = rect.height * 1.0 / image->height;

  // return landmarks
  for (int i = 0; i < SZ_LANDMARK_NUM; i++) {
    detection.landmark[i][0] = pose.landmarks.point[i].x / image->width;
    detection.landmark[i][1] = pose.landmarks.point[i].y / image->height;
  }

  // return head pose
  detection.yaw = pose.yaw;
  detection.pitch = pose.pitch;
  detection.roll = pose.roll;

  return true;
}

bool DetectTask::check(DetectionRatio detection, bool is_bgr) {
  if (!detection.is_valid_pose()) return false;

  if (is_bgr) {
    if (!is_stable(detection)) return false;

    static int invalid_count = 0;
    if (!detection.is_valid_position() || !detection.is_valid_size()) {
      if (Config::get_user().enable_temperature) {
        if (AudioTask::idle() && invalid_count++ > 20) {
          invalid_count = 0;
          emit tx_warn_distance();
        }
        emit tx_display_rectangle();
      }
      return false;
    }
    invalid_count = 0;
  }

  return true;
}

bool DetectTask::is_stable(DetectionRatio detection) {
  static int stable_counter = 0;

  static float x1 = 0;
  static float y1 = 0;
  static float w1 = 0;
  static float h1 = 0;

  auto cfg = Config::get_detect();

  float x2 = detection.x, y2 = detection.y;
  float w2 = detection.width, h2 = detection.height;

  if (x1 > x2 + w2 || y1 > y2 + h2 || x1 + w1 < x2 || y1 + h1 < y2)
    stable_counter = 0;
  else {
    float overlay_w = std::min(x1 + w1, x2 + w2) - std::max(x1, x2);
    float overlay_h = std::min(y1 + h1, y2 + h2) - std::max(y1, y2);
    float iou = overlay_w * overlay_h / (w1 * h1 + w2 * h2) * 2;

    if (iou >= cfg.min_tracking_iou)
      stable_counter++;
    else
      stable_counter = 0;
  }

  x1 = x2;
  y1 = y2;
  w1 = w2;
  h1 = h2;

  return stable_counter >= cfg.min_tracking_number;
}
