#include "camera_reader.h"
#include "sz_common.h"
#include "vi.h"

using namespace suanzi;

CameraReader::CameraReader(int cameralIndex, QObject *parent) {

  pvi_bgr_ = new Vi(DEV_IDX_BRG, PIPE_IDX_BRG, SONY_IMX307_MIPI_2M_30FPS_12BIT);
  pvpss_bgr_ = new Vpss(DEV_IDX_BRG, VPSS_CH_SIZES_BGR[0].width, VPSS_CH_SIZES_BGR[0].height);
  pvi_vpss_bgr_ = new Vi_Vpss(pvi_bgr_, pvpss_bgr_, VPSS_CH_SIZES_BGR,
                              CH_INDEXES_BGR, CH_ROTATES_BGR, sizeof(VPSS_CH_SIZES_BGR) / sizeof(SZ_SIZE));

  static Vo vo_bgr(0, VO_INTF_MIPI, VO_W, VO_H);
  static  Vi_Vpss_Vo vi_vpss_vo(pvi_vpss_bgr_, &vo_bgr);

  start();
}

CameraReader::~CameraReader() {
  delete pvi_bgr_;
  delete pvpss_bgr_;
  delete pvi_vpss_bgr_;
}

void CameraReader::run() {

  ImagePackage image_package1(VPSS_CH_SIZES_BGR[0], VPSS_CH_SIZES_BGR[2], VPSS_CH_SIZES_NIR[0], VPSS_CH_SIZES_NIR[1]);
  ImagePackage image_package2(VPSS_CH_SIZES_BGR[0], VPSS_CH_SIZES_BGR[2], VPSS_CH_SIZES_NIR[0], VPSS_CH_SIZES_NIR[1]);
  PingPangBuffer<ImagePackage> pingpang_buffer(&image_package1, &image_package2);

  while (1) {
    ImagePackage *pPing = pingpang_buffer.getPing();
    if (pvpss_bgr_->getYuvFrame(pPing->img_bgr_small, 2)) {
      pingpang_buffer.switchToPang();
      emit txFrame(&pingpang_buffer);

      printf("tx0 threadId=%x   %x\n", QThread::currentThreadId(), pPing);

    } else {
      QThread::msleep(1);
    }
  }
}
