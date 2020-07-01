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
    Vi *pvi_bgr_;
    Vpss *pvpss_bgr_;
    Vi_Vpss *pvi_vpss_bgr_;

    const int DEV_IDX_BRG=1;
    const int PIPE_IDX_BRG=2;
    const int DEV_IDX_NIR=0;
    const int PIPE_IDX_NIR=0;

    const Size VPSS_CH_SIZES_BGR[3] = {{1920, 1080}, {1080, 688}, {256, 256}};   // larger small
    const Size VPSS_CH_SIZES_NIR[2] = {{1920, 1080}, {256, 256}};   // larger small
    const int CH_INDEXES_BGR[3] = {0, 1, 2};
    const bool CH_ROTATES_BGR[3] = {false, true, true};
    const int CH_INDEXES_NIR[2] = {0, 1};
    const bool CH_ROTATES_NIR[2] = {false, false};

    const int VO_W = 800;
    const int VO_H = 1280;
    
};

}  // namespace suanzi

#endif
