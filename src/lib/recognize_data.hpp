#ifndef RECOGNIZE_DATA_H
#define RECOGNIZE_DATA_H

#include <mmzimage.h>

#include <QMetaType>
#include <opencv2/opencv.hpp>

#include "detection_float.h"
#include "image_package.hpp"
#include "ive.h"
#include "logger.hpp"
#include "quface/common.hpp"

namespace suanzi {

class RecognizeData : public ImagePackage {
 public:
  DetectionRatio bgr_detection;
  DetectionRatio nir_detection;
  bool is_alive;

  RecognizeData() {}

  RecognizeData(Size size_bgr_large, Size size_bgr_small, Size size_nir_large,
                Size size_nir_small)
      : ImagePackage(size_bgr_large, size_bgr_small, size_nir_large,
                     size_nir_small) {
    bgr_detection.b_valid = false;
    nir_detection.b_valid = false;
    is_alive = false;
  }

  ~RecognizeData() {}
};

}  // namespace suanzi

Q_DECLARE_METATYPE(suanzi::RecognizeData);

#endif
