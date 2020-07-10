#ifndef RECOGNIZE_DATA_H
#define RECOGNIZE_DATA_H

#include <mmzimage.h>

#include <QMetaType>
#include <opencv2/opencv.hpp>

#include "ive.h"
#include "logger.hpp"
#include "quface/common.hpp"
#include "detection_float.h"

namespace suanzi {

class RecognizeData {
 public:
  MmzImage *img_bgr_small;
  MmzImage *img_bgr_large;
  DetectionFloat detection;

  RecognizeData() {}

  RecognizeData(Size size_bgr_large, Size size_bgr_small) {
    img_bgr_small = new MmzImage(size_bgr_small.width, size_bgr_small.height,
                                 SZ_IMAGETYPE_NV21);
    img_bgr_large = new MmzImage(size_bgr_large.width, size_bgr_large.height,
                                 SZ_IMAGETYPE_NV21);
	detection.b_valid = false;
  }

  ~RecognizeData() {
    if (img_bgr_small) delete img_bgr_small;
    if (img_bgr_large) delete img_bgr_large;
  }


  void copy_to(RecognizeData &recognize_data) {
	img_bgr_large->copy_to(*recognize_data.img_bgr_large);
	img_bgr_small->copy_to(*recognize_data.img_bgr_small);
	recognize_data.detection = detection;
  }
  

  void bgrPacked2BgrPlanar(char *pDst, const char *pSrc, int width,
                           int height) {
    int len = width * height;
    char *pb = pDst;
    char *pg = pb + len;
    char *pr = pg + len;
    for (int i = 0; i < len; i++) {
      pb[i] = pSrc[i * 3];
      pg[i] = pSrc[i * 3 + 1];
      pr[i] = pSrc[i * 3 + 2];
    }
  }

  bool get_jpeg_buffer(const MmzImage *src_img, std::vector<SZ_BYTE> &buffer) {
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
};

}  // namespace suanzi

Q_DECLARE_METATYPE(suanzi::RecognizeData);

#endif
