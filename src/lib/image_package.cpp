#include "image_package.hpp"

using namespace suanzi;

ImagePackage::ImagePackage() { frame_idx = 0; }

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

void ImagePackage::copy_to(ImagePackage &pkg) {
  img_bgr_large->copy_to(*pkg.img_bgr_large);
  img_bgr_small->copy_to(*pkg.img_bgr_small);
  img_nir_large->copy_to(*pkg.img_nir_large);
  img_nir_small->copy_to(*pkg.img_nir_small);

  pkg.frame_idx = frame_idx;
}

bool ImagePackage::get_jpeg_buffer(const MmzImage *src_img,
                                   std::vector<SZ_BYTE> &buffer) {
  MmzImage *dest_img =
      new MmzImage(src_img->width, src_img->height, SZ_IMAGETYPE_BGR_PACKAGE);

  Ive *ive = Ive::getInstance();
  if (!ive->yuv2RgbPacked(dest_img, src_img, true)) {
    SZ_LOG_ERROR("IVE yuv2RgbPacked failed {}");
    delete dest_img;
    return false;
  }

  cv::Mat mat =
      cv::Mat(src_img->height, src_img->width, CV_8UC3, dest_img->pData);
  if (!cv::imencode(".jpg", mat, buffer)) {
    SZ_LOG_ERROR("Encode jpg failed");
    delete dest_img;
    return false;
  }
  delete dest_img;
  return true;
}