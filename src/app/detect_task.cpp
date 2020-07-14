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

void DetectTask::copy_buffer(ImagePackage *pang, DetectionFloat &bgr_detection,
                             DetectionFloat &nir_detection) {
  if (pingpang_buffer_ == nullptr) {
    recognize_data1_ =
        new RecognizeData({(SZ_UINT32)pang->img_bgr_large->width,
                           (SZ_UINT32)pang->img_bgr_large->height},
                          {(SZ_UINT32)pang->img_bgr_small->width,
                           (SZ_UINT32)pang->img_bgr_small->height},
                          {(SZ_UINT32)pang->img_nir_large->width,
                           (SZ_UINT32)pang->img_nir_large->height},
                          {(SZ_UINT32)pang->img_nir_small->width,
                           (SZ_UINT32)pang->img_nir_small->height});
    recognize_data2_ =
        new RecognizeData({(SZ_UINT32)pang->img_bgr_large->width,
                           (SZ_UINT32)pang->img_bgr_large->height},
                          {(SZ_UINT32)pang->img_bgr_small->width,
                           (SZ_UINT32)pang->img_bgr_small->height},
                          {(SZ_UINT32)pang->img_nir_large->width,
                           (SZ_UINT32)pang->img_nir_large->height},
                          {(SZ_UINT32)pang->img_nir_small->width,
                           (SZ_UINT32)pang->img_nir_small->height});
    pingpang_buffer_ =
        new PingPangBuffer<RecognizeData>(recognize_data1_, recognize_data2_);
  }
  RecognizeData *ping = pingpang_buffer_->get_ping();
  pang->copy_to(*ping);
  ping->bgr_detection = bgr_detection;
  ping->nir_detection = nir_detection;
}

void DetectTask::rx_frame(PingPangBuffer<ImagePackage> *buffer) {
  // SZ_LOG_DEBUG("rx_frame");
  ImagePackage *pang = buffer->get_pang();
  SZ_RETCODE ret;

  // 256x256  7ms
  std::vector<suanzi::FaceDetection> bgr_detections;
  ret = face_detector_->detect(
      (const SVP_IMAGE_S *)pang->img_bgr_small->pImplData, bgr_detections,
      config_->detect.threshold, config_->detect.min_face_size);
  if (ret != SZ_RETCODE_OK) {
    SZ_LOG_ERROR("Detect bgr error ret={}", ret);
  }

  std::vector<suanzi::FaceDetection> nir_detections;
  ret = face_detector_->detect(
      (const SVP_IMAGE_S *)pang->img_nir_small->pImplData, nir_detections);
  if (ret != SZ_RETCODE_OK) {
    SZ_LOG_ERROR("Detect nir error ret={}", ret);
  }

  int width = pang->img_bgr_small->width;
  int height = pang->img_bgr_small->height;
  bool valid_face = false;
  DetectionFloat largest_bgr_face;
  DetectionFloat largest_nir_face;

  if (bgr_detections.size() > 0 && nir_detections.size() > 0) {
    valid_face = false;
    largest_bgr_face = select_face(bgr_detections, width, height);
    valid_face = largest_bgr_face.b_valid;
    largest_bgr_face.b_valid = true;
    emit tx_display(largest_bgr_face);

    width = pang->img_nir_small->width;
    height = pang->img_nir_small->height;
    largest_nir_face = select_face(nir_detections, width, height);
  }

  if (valid_face) {
    copy_buffer(pang, largest_bgr_face, largest_nir_face);
    if (b_tx_ok_) {
      b_tx_ok_ = false;
      b_data_ready_ = false;
      emit tx_frame(pingpang_buffer_);
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
      emit tx_frame(pingpang_buffer_);
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

  DetectionFloat detection;
  auto rect = detections[max_id].bbox;
  detection.x = rect.x * 1.0 / width;
  detection.y = rect.y * 1.0 / height;
  detection.width = rect.width * 1.0 / width;
  detection.height = rect.height * 0.8 / height;  // remove neck
  if (isnan(detections[max_id].yaw) ||
      detections[max_id].yaw > config_->detect.max_yaw ||
      detections[max_id].yaw < config_->detect.min_yaw ||
      isnan(detections[max_id].pitch) ||
      detections[max_id].pitch > config_->detect.max_pitch ||
      detections[max_id].pitch < config_->detect.min_pitch)
    detection.b_valid = false;
  else
    detection.b_valid = true;

  for (int i = 0; i < 5; i++) {
    detection.landmark[i][0] = detections[max_id].landmarks.point[i].x / width;
    detection.landmark[i][1] = detections[max_id].landmarks.point[i].y / height;
  }

  return detection;
}
