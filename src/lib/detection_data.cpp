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

    bool position_valid = bgr_detection_.x > 0.1 && bgr_detection_.y > 0.05 &&
                          bgr_detection_.x + bgr_detection_.width < 0.9 &&
                          bgr_detection_.y + bgr_detection_.height < 0.95;
    return pose_valid && position_valid;
  }
  return false;
}

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
  return false;
}
