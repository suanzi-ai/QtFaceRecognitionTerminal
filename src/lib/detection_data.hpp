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

  void scale(int x_scale, int y_scale, FaceDetection &detection,
             FacePose &pose);
  bool is_overlap(DetectionRatio other);
  bool is_valid_pose();
  bool is_valid_position();
  bool is_valid_size();
};

class DetectionData : public ImagePackage {
 public:
  DetectionData();
  DetectionData(const ImagePackage *pkg);
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

  bool bgr_face_valid_;
  bool nir_face_valid_;
};

}  // namespace suanzi

Q_DECLARE_METATYPE(suanzi::DetectionRatio);
Q_DECLARE_METATYPE(suanzi::DetectionData);

#endif
