#include "detection_data.hpp"

#include "config.hpp"

using namespace suanzi;

DetectionData::DetectionData() {
  bgr_face_detected_ = false;
  nir_face_detected_ = false;
}

DetectionData::DetectionData(Size size_bgr_large, Size size_bgr_small,
                             Size size_nir_large, Size size_nir_small)
    : ImagePackage(size_bgr_large, size_bgr_small, size_nir_large,
                   size_nir_small) {
  bgr_face_detected_ = false;
  nir_face_detected_ = false;
}

DetectionData::~DetectionData() {}

bool DetectionData::bgr_face_detected() { return bgr_face_detected_; }

bool DetectionData::nir_face_detected() { return nir_face_detected_; }

bool DetectionData::bgr_face_valid() {
  if (bgr_face_detected()) {
    auto cfg = Config::get_detect();
    bool pose_valid =
        !isnanf(bgr_detection_.yaw) && !isnanf(bgr_detection_.pitch) &&
        cfg.min_yaw < bgr_detection_.yaw && bgr_detection_.yaw < cfg.max_yaw &&
        cfg.min_pitch < bgr_detection_.pitch &&
        bgr_detection_.pitch < cfg.max_pitch;

    bool position_valid = bgr_detection_.x > 0.15 && bgr_detection_.y > 0.15 &&
                          bgr_detection_.x + bgr_detection_.width < 0.85 &&
                          bgr_detection_.y + bgr_detection_.height < 0.85;
    return pose_valid && position_valid;
  }
  return false;
}

// nyy
bool DetectionData::nir_face_valid() {
  if (bgr_face_detected() && nir_face_detected()) {
    float x1 = bgr_detection_.x, x2 = nir_detection_.x;
    float y1 = bgr_detection_.y, y2 = nir_detection_.y;
    float w1 = bgr_detection_.width, w2 = nir_detection_.width;
    float h1 = bgr_detection_.height, h2 = nir_detection_.height;

    if (x1 > x2 + w2 || y1 > y2 + h2 || x1 + w1 < x2 || y1 + h1 < y2)
      return false;

    float overlay_w = std::min(x1 + w1, x2 + w2) - std::max(x1, x2);
    float overlay_h = std::min(y1 + h1, y2 + h2) - std::max(y1, y2);
    float iou = overlay_w * overlay_h / (w1 * h1 + w2 * h2) * 2;
    return iou > 0.45;
  }
  return false;
}
