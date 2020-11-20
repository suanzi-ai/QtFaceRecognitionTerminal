#include "image_package.hpp"

using namespace suanzi;

ImagePackage::ImagePackage() { frame_idx = 0; }

ImagePackage::ImagePackage(const ImagePackage* pkg) {
  img_bgr_small = new MmzImage(pkg->img_bgr_small->width,
                               pkg->img_bgr_small->height, SZ_IMAGETYPE_NV21);
  img_bgr_large = new MmzImage(pkg->img_bgr_large->width,
                               pkg->img_bgr_large->height, SZ_IMAGETYPE_NV21);

  img_nir_small = new MmzImage(pkg->img_nir_small->width,
                               pkg->img_nir_small->height, SZ_IMAGETYPE_NV21);
  img_nir_large = new MmzImage(pkg->img_nir_large->width,
                               pkg->img_nir_large->height, SZ_IMAGETYPE_NV21);

  frame_idx = pkg->frame_idx;
}

ImagePackage::ImagePackage(Size size_bgr_large, Size size_bgr_small,
                           Size size_nir_large, Size size_nir_small) {
  img_bgr_small = new MmzImage(size_bgr_small.width, size_bgr_small.height,
                               SZ_IMAGETYPE_NV21);
  img_bgr_large = new MmzImage(size_bgr_large.width, size_bgr_large.height,
                               SZ_IMAGETYPE_NV21);

  img_nir_small = new MmzImage(size_nir_small.width, size_nir_small.height,
                               SZ_IMAGETYPE_NV21);
  img_nir_large = new MmzImage(size_nir_large.width, size_nir_large.height,
                               SZ_IMAGETYPE_NV21);

  frame_idx = 0;
}

ImagePackage::~ImagePackage() {
  if (img_bgr_small) delete img_bgr_small;
  if (img_bgr_large) delete img_bgr_large;
  if (img_nir_small) delete img_nir_small;
  if (img_nir_large) delete img_nir_large;
}

void ImagePackage::copy_to(ImagePackage& pkg) {
  img_bgr_large->copy_to(*pkg.img_bgr_large);
  img_bgr_small->copy_to(*pkg.img_bgr_small);
  img_nir_large->copy_to(*pkg.img_nir_large);
  img_nir_small->copy_to(*pkg.img_nir_small);

  pkg.frame_idx = frame_idx;
}
