#ifndef IMAGE_PACKAGE_H
#define IMAGE_PACKAGE_H

#include <QMetaType>
#include <mmzimage.h>
#include "quface/common.hpp"

namespace suanzi {

class ImagePackage {


 public:
    int frame_idx;
    MmzImage* img_bgr_small;
    MmzImage* img_bgr_large;
    MmzImage* img_nir_small;
    MmzImage* img_nir_large;

    ImagePackage(){}

    ImagePackage(Size size_bgr_large, Size size_bgr_small, Size size_nir_large,Size size_nir_small)
    {
        img_bgr_small = new MmzImage(size_bgr_small.width, size_bgr_small.height, SZ_IMAGETYPE_NV21);
        img_bgr_large = new MmzImage(size_bgr_large.width, size_bgr_large.height, SZ_IMAGETYPE_NV21);

        img_nir_small = new MmzImage(size_nir_small.width, size_nir_small.height, SZ_IMAGETYPE_NV21);
        img_nir_large = new MmzImage(size_nir_large.width, size_nir_large.height, SZ_IMAGETYPE_NV21);

        frame_idx = 0;
    }

    ~ImagePackage()
    {
        if(img_bgr_small) delete  img_bgr_small;
        if(img_bgr_large) delete  img_bgr_large;
        if(img_nir_small) delete  img_nir_small;
        if(img_nir_large) delete  img_nir_large;
    }
};

}

Q_DECLARE_METATYPE(suanzi::ImagePackage);

#endif
