#ifndef IMAGE_PACKAGE_H
#define IMAGE_PACKAGE_H

#include <QMetaType>
#include <opencv2/opencv.hpp>

#include "ive.h"
#include "logger.hpp"
#include "mmzimage.h"
#include "quface/common.hpp"

namespace suanzi {

class ImagePackage {
 public:
  ImagePackage();
  ImagePackage(Size size_bgr_large, Size size_bgr_small, Size size_nir_large,
               Size size_nir_small);
  ~ImagePackage();

  void copy_to(ImagePackage &pkg);
  bool get_jpeg_buffer(const MmzImage *src_img, std::vector<SZ_BYTE> &buffer);

 public:
  int frame_idx;
  MmzImage *img_bgr_small;
  MmzImage *img_bgr_large;
  MmzImage *img_nir_small;
  MmzImage *img_nir_large;
};

}  // namespace suanzi

Q_DECLARE_METATYPE(suanzi::ImagePackage);

#endif
