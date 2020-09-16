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

bool DetectionRatio::is_valid() {
  auto detect = Config::get_detect();
  auto temperature = Config::get_temperature();
  auto user = Config::get_user();
  bool pose_valid = !std::isnan(yaw) && !std::isnan(pitch) &&
                    !std::isnan(roll) && detect.min_yaw < yaw &&
                    yaw < detect.max_yaw && detect.min_pitch < pitch &&
                    pitch < detect.max_pitch && detect.min_roll < roll &&
                    roll < detect.max_roll;
  bool position_valid;

  if (user.disabled_temperature)
    position_valid =
        x > 0.01 && y > 0.01 && x + width < 0.99 && y + height < 0.99;
  else {
    float face_width, face_height, face_x, face_y, distance;

    face_width = temperature.device_face_width;
    face_height = temperature.device_face_height;
    face_x = temperature.device_face_x;
    face_y = temperature.device_face_y;
    distance = temperature.temperature_distance;

    position_valid =
        x > face_x && y > face_y && x + width < face_x + face_width &&
        y + height - 0.04 < face_height + face_y &&
        width > face_width * distance &&
        height >
            face_height * distance;  // 补偿0.04，以让人脸完全进入框中也能识别
  }
  return pose_valid && position_valid;
}

DetectionData::DetectionData() {
  bgr_face_detected_ = false;
  nir_face_detected_ = false;
}

DetectionData::DetectionData(const ImagePackage *pkg) : ImagePackage(pkg) {
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
  bool ret = false;
  if (bgr_face_detected() && bgr_detection_.is_valid()) {
    static float x1 = 0;
    static float y1 = 0;
    static float w1 = 0;
    static float h1 = 0;

    static int stable_counter = 0;

    auto cfg = Config::get_detect();

    float x2 = bgr_detection_.x, y2 = bgr_detection_.y;
    float w2 = bgr_detection_.width, h2 = bgr_detection_.height;

    if (x1 > x2 + w2 || y1 > y2 + h2 || x1 + w1 < x2 || y1 + h1 < y2)
      ret = false;
    else {
      float overlay_w = std::min(x1 + w1, x2 + w2) - std::max(x1, x2);
      float overlay_h = std::min(y1 + h1, y2 + h2) - std::max(y1, y2);
      float iou = overlay_w * overlay_h / (w1 * h1 + w2 * h2) * 2;

      ret = iou >= cfg.min_tracking_iou;
    }

    x1 = x2;
    y1 = y2;
    w1 = w2;
    h1 = h2;

    if (ret)
      stable_counter++;
    else
      stable_counter = 0;

    ret = stable_counter >= cfg.min_tracking_number;
  }

  return ret;
}

bool DetectionData::nir_face_valid() {
  return bgr_face_detected() && nir_face_detected() &&
         nir_detection_.is_overlap(bgr_detection_);
}
