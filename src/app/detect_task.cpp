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

DetectTask::DetectTask(FaceDetectorPtr detector, Config::ptr config,
                       QThread *thread, QObject *parent)
    : face_detector_(detector), config_(config) {
  b_tx_ok_ = true;
  b_data_ready_ = false;
  recognize_data1_ = nullptr;
  recognize_data2_ = nullptr;
  pingpang_buffer_ = nullptr;

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
  if (recognize_data1_ != nullptr) delete recognize_data1_;
  if (recognize_data2_ != nullptr) delete recognize_data2_;
  if (pingpang_buffer_ != nullptr) delete pingpang_buffer_;
}

void DetectTask::copy_buffer(ImagePackage *pang, DetectionFloat &detection) {
  if (pingpang_buffer_ == nullptr) {
    recognize_data1_ = new RecognizeData(
        {pang->img_bgr_large->width, pang->img_bgr_large->height},
        {pang->img_bgr_small->width, pang->img_bgr_small->height});
    recognize_data2_ = new RecognizeData(
        {pang->img_bgr_large->width, pang->img_bgr_large->height},
        {pang->img_bgr_small->width, pang->img_bgr_small->height});
    pingpang_buffer_ =
        new PingPangBuffer<RecognizeData>(recognize_data1_, recognize_data2_);
  }
  RecognizeData *ping = pingpang_buffer_->get_ping();
  pang->copy_to(*ping);
  ping->detection = detection;
}

void DetectTask::rx_frame(PingPangBuffer<ImagePackage> *buffer) {
  // SZ_LOG_DEBUG("rx_frame");
  ImagePackage *pang = buffer->get_pang();
  // 256x256  7ms
  std::vector<suanzi::FaceDetection> detections;
  SZ_RETCODE ret = face_detector_->detect(
      (const SVP_IMAGE_S *)pang->img_bgr_small->pImplData, detections);
  if (ret != SZ_RETCODE_OK) {
    SZ_LOG_ERROR("Detect error ret={}", ret);
  }

  // if face detected
  int width = pang->img_bgr_small->width;
  int height = pang->img_bgr_small->height;
  bool valid_face = false;
  DetectionFloat largest_face;
  if (ret == SZ_RETCODE_OK && detections.size() > 0) {
    largest_face = select_face(detections, width, height);
    valid_face = largest_face.b_valid;
    largest_face.b_valid = true;
    emit tx_display(largest_face);
  }

  if (valid_face) {
    copy_buffer(pang, largest_face);

    // send detection if recognized finished
    if (b_tx_ok_) {
      b_tx_ok_ = false;
      b_data_ready_ = false;
      emit tx_recognize(pingpang_buffer_);
    } else {
      b_data_ready_ = true;
    }
  } else {
    DetectionFloat no_face;
    no_face.b_valid = false;
    emit tx_display(no_face);
    if (b_tx_ok_ && b_data_ready_) {
      b_tx_ok_ = false;
      b_data_ready_ = false;
      emit tx_recognize(pingpang_buffer_);
    } else {
      emit tx_no_frame();
    }
  }
  buffer->switch_buffer();
  // SZ_LOG_DEBUG("tx_finish");
  emit tx_finish();
}

void DetectTask::rx_finish() { b_tx_ok_ = true; }

DetectionFloat DetectTask::select_face(
    std::vector<suanzi::FaceDetection> &detections, int width, int height) {
  int max_id = 0;
  float max_area = detections[0].bbox.width * detections[0].bbox.height;
  for (int i = 1; i < detections.size(); i++) {
    float area = detections[i].bbox.width * detections[i].bbox.height;
    if (area > max_area) {
      max_id = i;
      max_area = area;
    }
  }

  DetectionFloat detection_bgr;
  auto rect = detections[max_id].bbox;
  detection_bgr.x = rect.x * 1.0 / width;
  detection_bgr.y = rect.y * 1.0 / height;
  detection_bgr.width = rect.width * 1.0 / width;
  detection_bgr.height = rect.height * 0.8 / height;  // remove neck
  if (isnan(detections[max_id].yaw) ||
      detections[max_id].yaw > config_->detect.max_yaw ||
      detections[max_id].yaw < config_->detect.min_yaw ||
      isnan(detections[max_id].pitch) ||
      detections[max_id].pitch > config_->detect.max_pitch ||
      detections[max_id].pitch < config_->detect.min_pitch)
    detection_bgr.b_valid = false;
  else
    detection_bgr.b_valid = true;

  for (int i = 0; i < 5; i++) {
    detection_bgr.landmark[i][0] =
        detections[max_id].landmarks.point[i].x / width;
    detection_bgr.landmark[i][1] =
        detections[max_id].landmarks.point[i].y / height;
  }

  return detection_bgr;
}
