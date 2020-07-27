#ifndef DETECTION_DATA_H
#define DETECTION_DATA_H

#include <QMetaType>

#include "image_package.hpp"
#include "quface/common.hpp"

namespace suanzi {

struct DetectionRatio {
  float x;
  float y;
  float width;
  float height;
  float landmark[SZ_LANDMARK_NUM][2];
  float yaw;
  float pitch;
  float roll;

  void scale(int x_scale, int y_scale, FaceDetection &detection, FacePose &pose) {
    suanzi::FaceDetection face_detection;
    face_detection.bbox.x = x * x_scale;
    face_detection.bbox.y = y * y_scale;
    face_detection.bbox.width = width * x_scale;
    face_detection.bbox.height = height * y_scale;

    for (int i = 0; i < SZ_LANDMARK_NUM; i++) {
      pose.landmarks.point[i].x =
          landmark[i][0] * x_scale;
      pose.landmarks.point[i].y =
          landmark[i][1] * y_scale;
    }
    pose.yaw = yaw;
    pose.pitch = pitch;
    pose.roll = roll;
  }
};

class DetectionData : public ImagePackage {
 public:
  DetectionData();
  DetectionData(Size size_bgr_large, Size size_bgr_small, Size size_nir_large,
                Size size_nir_small);

  ~DetectionData();

  bool bgr_face_detected();
  bool nir_face_detected();

  bool bgr_face_valid();
  bool nir_face_valid();

 public:
  DetectionRatio bgr_detection_;
  DetectionRatio nir_detection_;

  bool bgr_face_detected_;
  bool nir_face_detected_;

};

}  // namespace suanzi

Q_DECLARE_METATYPE(suanzi::DetectionRatio);
Q_DECLARE_METATYPE(suanzi::DetectionData);

#endif
