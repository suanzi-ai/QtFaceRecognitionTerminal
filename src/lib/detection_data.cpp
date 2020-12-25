#include "detection_data.hpp"

#include <cmath>

#include "config.hpp"

using namespace suanzi;

void DetectionRatio::scale(int x_scale, int y_scale, FaceDetection &detection,
                           FacePose &pose) {
  detection.bbox.x = x * x_scale;
  detection.bbox.y = y * y_scale;
  detection.bbox.width = width * x_scale;
  detection.bbox.height = height * y_scale;

  for (int i = 0; i < SZ_LANDMARK_NUM; i++) {
    pose.landmarks.point[i].x = landmark[i][0] * x_scale;
    pose.landmarks.point[i].y = landmark[i][1] * y_scale;
  }
  pose.yaw = yaw;
  pose.pitch = pitch;
  pose.roll = roll;
}

bool DetectionRatio::is_overlap(DetectionRatio other) {
  float x1 = x, x2 = other.x;
  float y1 = y, y2 = other.y;
  float w1 = width, w2 = other.width;
  float h1 = height, h2 = other.height;

  if (x1 > x2 + w2 || y1 > y2 + h2 || x1 + w1 < x2 || y1 + h1 < y2)
    return false;

  float overlay_w = std::min(x1 + w1, x2 + w2) - std::max(x1, x2);
  float overlay_h = std::min(y1 + h1, y2 + h2) - std::max(y1, y2);
  float iou = overlay_w * overlay_h / (w1 * h1 + w2 * h2) * 2;

  auto cfg = Config::get_liveness();
  bool ret = iou > cfg.min_iou_between_bgr &&
             w1 / w2 >= cfg.min_width_ratio_between_bgr &&
             w1 / w2 <= cfg.max_width_ratio_between_bgr &&
             h1 / h2 >= cfg.min_height_ratio_between_bgr &&
             h1 / h2 <= cfg.max_height_ratio_between_bgr;
  if (!ret) {
    SZ_LOG_DEBUG("bgr=[{:.2f}, {:.2f}, {:.2f}, {:.2f}]", x1, y1, w1, h1);
    SZ_LOG_DEBUG("nir=[{:.2f}, {:.2f}, {:.2f}, {:.2f}]", x2, y2, w2, h2);
    SZ_LOG_DEBUG("iou = {:.2f}, w1 / w1 = {:.2f}, h1 / h2 = {:.2f}", iou,
                 w1 / w2, h1 / h2);
  }
  return ret;
}

bool DetectionRatio::is_valid_pose() {
  auto detect = Config::get_detect();
  return !std::isnan(yaw) && !std::isnan(pitch) && !std::isnan(roll) &&
         detect.min_yaw < yaw && yaw < detect.max_yaw &&
         detect.min_pitch < pitch && pitch < detect.max_pitch &&
         detect.min_roll < roll && roll < detect.max_roll;
}

bool DetectionRatio::is_valid_position() {
  auto user = Config::get_user();
  auto temperature = Config::get_temperature();

  float min_x, min_y, max_x, max_y;
  if (!user.enable_temperature) {
    min_x = min_y = 0.01;
    max_x = max_y = 0.99;
  } else {
    /*min_x = 0.15;
    min_y = 0.05;
    max_x = 0.85;
    max_y = 0.95;*/
    min_x = temperature.temperature_area_x;
    min_y = temperature.temperature_area_y;
    max_x = min_x + temperature.temperature_area_width;
    max_y = min_y + temperature.temperature_area_height;
  }
  return x > min_x && y > min_y && x + width < max_x && y + height < max_y;
}

bool DetectionRatio::is_valid_size() {
  auto temperature = Config::get_temperature();
  auto user = Config::get_user();
  if (!user.enable_temperature)
    return true;
  else {
    float min_width = temperature.min_face_width;
    float min_height = temperature.min_face_height;
    return width > min_width && height > min_height;
  }
}

DetectionData::DetectionData() {
  bgr_face_detected_ = false;
  nir_face_detected_ = false;

  bgr_face_valid_ = false;
  nir_face_valid_ = false;
}

DetectionData::DetectionData(const ImagePackage *pkg) : ImagePackage(pkg) {
  bgr_face_detected_ = false;
  nir_face_detected_ = false;

  bgr_face_valid_ = false;
  nir_face_valid_ = false;
}

DetectionData::DetectionData(Size size_bgr_large, Size size_bgr_small,
                             Size size_nir_large, Size size_nir_small)
    : ImagePackage(size_bgr_large, size_bgr_small, size_nir_large,
                   size_nir_small) {
  bgr_face_detected_ = false;
  nir_face_detected_ = false;

  bgr_face_valid_ = false;
  nir_face_valid_ = false;
}

DetectionData::~DetectionData() {}

bool DetectionData::bgr_face_detected() { return bgr_face_detected_; }

bool DetectionData::nir_face_detected() { return nir_face_detected_; }

bool DetectionData::bgr_face_valid() {
  return bgr_face_detected_ && bgr_face_valid_;
}

bool DetectionData::nir_face_valid() {
  return bgr_face_detected_ && nir_face_detected_ &&
         nir_detection_.is_overlap(bgr_detection_);
}
