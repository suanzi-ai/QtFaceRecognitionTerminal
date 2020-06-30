#ifndef CAMERA_READER_H
#define CAMERA_READER_H

#include <QThread>
#include <QImage>
#include <QSharedPointer>
#include "vi.h"
#include "vpss.h"
#include "vi_vpss.h"
#include "vi_vpss_vo.h"
#include "vo.h"
#include "pingpangbuffer.h"
#include "image_package.h"

namespace suanzi {

class CameraReader : QThread {

    Q_OBJECT
 public:
    CameraReader(int cameralIndex, QObject *parent = nullptr);
    ~CameraReader();

 private:
    void run();

 signals:
  void txFrame(PingPangBuffer<ImagePackage> *buffer);

 private:
    Vi *pVi_bgr_;
    Vpss *pVpss_bgr_;
    Vi_Vpss *pVi_Vpss_bgr_;

    const int DEV_IDX_BRG=1;
    const int PIPE_IDX_BRG=2;

    const int DEV_IDX_NIR=0;
    const int PIPE_IDX_NIR=0;

    const SZ_SIZE vpss_ch_sizes_bgr[2] = {{1920, 1080}, {256, 256}};
    const SZ_SIZE vpss_ch_sizes_nir[2] = {{1920, 1080}, {256, 256}};

    const int ch_indexes_bgr[2] = {0, 1};
    const bool ch_rotates_bgr[2] = {false, false};

    const int ch_indexes_nir[2] = {0, 1};
    const bool ch_rotates_nir[2] = {false, false};

    const int VO_W = 800;
    const int VO_H = 1280;
    
};

}  // namespace suanzi

#endif
