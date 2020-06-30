#ifndef IMAGE_PACKAGE_H
#define IMAGE_PACKAGE_H

#include <QMetaType>
#include <mmzimage.h>

class ImagePackage {

 public:
    int frame_idx;
    MmzImage* img_bgr_small;
    MmzImage* img_bgr_large;
    MmzImage* img_nir_small;
    MmzImage* img_nir_large;
};

Q_DECLARE_METATYPE(ImagePackage);

#endif
