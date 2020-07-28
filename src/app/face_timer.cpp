#include "face_timer.hpp"

#include "config.hpp"

using namespace suanzi;

FaceTimer::FaceTimer(QThread *thread, QObject *parent) {
  // Create thread
  if (thread == nullptr) {
    static QThread new_thread;
    moveToThread(&new_thread);
    new_thread.start();
  } else {
    moveToThread(thread);
    thread->start();
  }

  disappear_counter_ = 0;
  disappear_duration_ = 0;
  invalid_pose_counter_ = 0;

  last_detection_.width = 0;
  last_detection_.height = 0;
}

FaceTimer::~FaceTimer() {}

void FaceTimer::rx_frame(PingPangBuffer<DetectionData> *buffer) {
  DetectionData *input = buffer->get_pang();

  if (!input->bgr_face_detected() && !input->nir_face_detected()) {
    disappear_counter_++;
    if (disappear_counter_ == Config::get_extract().max_lost_age)
      disappear_begin_ = std::chrono::steady_clock::now();

    if (disappear_counter_ >= Config::get_extract().max_lost_age) {
      auto current_clock = std::chrono::steady_clock::now();
      disappear_duration_ = std::chrono::duration_cast<std::chrono::seconds>(
                                current_clock - disappear_begin_)
                                .count();
      emit tx_face_disappear(disappear_duration_);
      last_detection_.width = 0;
      last_detection_.height = 0;
    }
  } else {
    if (disappear_counter_ >= Config::get_extract().max_lost_age) {
      emit tx_face_appear(disappear_duration_);

      disappear_counter_ = 0;
      disappear_duration_ = 0;
    }
  }

  if (!input->bgr_face_valid()) {
    invalid_pose_counter_ += 1;
    if (invalid_pose_counter_ > Config::get_extract().max_lost_age / 2) {
      emit tx_face_reset();
      invalid_pose_counter_ = 0;
      last_detection_.width = 0;
      last_detection_.height = 0;
    }
  } else if (!track(input->bgr_detection_)) {
    emit tx_face_reset();
    invalid_pose_counter_ = 0;
  } else {
    invalid_pose_counter_ = 0;
  }
}

bool FaceTimer::track(DetectionRatio bgr_detection) {
  float x1 = last_detection_.x, x2 = bgr_detection.x;
  float y1 = last_detection_.y, y2 = bgr_detection.y;
  float w1 = last_detection_.width, w2 = bgr_detection.width;
  float h1 = last_detection_.height, h2 = bgr_detection.height;

  last_detection_ = bgr_detection;

  if (x1 > x2 + w2 || y1 > y2 + h2 || x1 + w1 < x2 || y1 + h1 < y2)
    return false;

  float overlay_w = std::min(x1 + w1, x2 + w2) - std::max(x1, x2);
  float overlay_h = std::min(y1 + h1, y2 + h2) - std::max(y1, y2);
  float iou = overlay_w * overlay_h / (w1 * h1 + w2 * h2) * 2;

  bool ret = iou > 0.8;
  // if (!ret) SZ_LOG_DEBUG("tracking iou = {}", iou);
  return ret;
}