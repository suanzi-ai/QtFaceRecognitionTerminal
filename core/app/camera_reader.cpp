#include "camera_reader.h"
#include "sz_common.h"
#include "vi.h"

using namespace suanzi;

CameraReader::CameraReader(int cameralIndex, QObject *parent) {

  // if (cameralIndex == 1) {
  // } else {
  //     pVi_ = new Vi(cameralIndex, cameralIndex,
  //     SONY_IMX307_MIPI_2M_30FPS_12BIT);
  // }
  // bool rotates[2] = {false, false};
  // SZ_SIZE vpssChSizes[2] = {{1920, 1080}, {256, 256}};
  // int chIndexes[2] = {0, 1};

  pVi_bgr_ = new Vi(DEV_IDX_BRG, PIPE_IDX_BRG, SONY_IMX307_MIPI_2M_30FPS_12BIT);
  pVpss_bgr_ = new Vpss(DEV_IDX_BRG, vpss_ch_sizes_bgr[0].width, vpss_ch_sizes_bgr[0].height);
  pVi_Vpss_bgr_ = new Vi_Vpss(pVi_bgr_, pVpss_bgr_, vpss_ch_sizes_bgr,
                              ch_indexes_bgr, ch_rotates_bgr, 2);

  static Vo vo(0, VO_INTF_MIPI, VO_W, VO_H);
  static Vi_Vpss_Vo vi_vpss_vo(pVi_Vpss_bgr_, &vo);

  start();
}

CameraReader::~CameraReader() {
  delete pVi_bgr_;
  delete pVpss_bgr_;
  delete pVi_Vpss_bgr_;
}

void CameraReader::run() {

  ImagePackage image_package[2];
   image_package[0].img_bgr_small = new MmzImage(256, 256, SZ_IMAGETYPE_NV21);
   image_package[1].img_bgr_small = new MmzImage(256, 256, SZ_IMAGETYPE_NV21);
//   MmzImage ping(256, 256, SZ_IMAGETYPE_NV21);
//   MmzImage pang(256, 256, SZ_IMAGETYPE_NV21);
  PingPangBuffer<ImagePackage> pingpang(&image_package[0], &image_package[1]);
  while (1) {
    ImagePackage *pPing = pingpang.getPing();
    if (pVpss_bgr_->getYuvFrame(pPing->img_bgr_small, 1)) {
      pingpang.switchToPang();
      emit txFrame(&pingpang);
      // printf("tx0 threadId=%x   %x\n", QThread::currentThreadId(), pPing);
    } else {
      QThread::msleep(1);
    }
  }
}
