#ifndef IMAGE_PACKAGE_H
#define IMAGE_PACKAGE_H

#include <QMetaType>

#include <opencv2/opencv.hpp>

#include <quface-io/mmzimage.hpp>
#include <quface/common.hpp>
#include <quface/logger.hpp>

namespace suanzi {
using namespace io;

class ImagePackage {
 public:
  ImagePackage();
  ImagePackage(const ImagePackage *pkg);
  ImagePackage(Size size_bgr_large, Size size_bgr_small, Size size_nir_large,
               Size size_nir_small);
  ~ImagePackage();

  void copy_to(ImagePackage &pkg);

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
