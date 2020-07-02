#include "camera_reader.h"

#include <QDebug>
#include <chrono>
#include <ctime>
#include <iostream>

#include "quface/common.hpp"
#include "vi.h"

using namespace suanzi;

CameraReader::CameraReader(int cameralIndex, QObject *parent) {
  pvi_bgr_ = new Vi(DEV_IDX_BRG, PIPE_IDX_BRG, SONY_IMX307_MIPI_2M_30FPS_12BIT);
  pvpss_bgr_ = new Vpss(DEV_IDX_BRG, VPSS_CH_SIZES_BGR[0].width,
                        VPSS_CH_SIZES_BGR[0].height);
  pvi_vpss_bgr_ =
      new Vi_Vpss(pvi_bgr_, pvpss_bgr_, VPSS_CH_SIZES_BGR, CH_INDEXES_BGR,
                  CH_ROTATES_BGR, sizeof(VPSS_CH_SIZES_BGR) / sizeof(Size));

  static Vo vo_bgr(0, VO_INTF_MIPI, VO_W, VO_H);
  static Vi_Vpss_Vo vi_vpss_vo(pvi_vpss_bgr_, &vo_bgr);

  start();
}

CameraReader::~CameraReader() {
  delete pvi_bgr_;
  delete pvpss_bgr_;
  delete pvi_vpss_bgr_;
}

void CameraReader::run() {
  Size size_bgr_1 = VPSS_CH_SIZES_BGR[1];
  Size size_bgr_2 = VPSS_CH_SIZES_BGR[2];
  if (CH_INDEXES_BGR[1]) {
    size_bgr_1.height = VPSS_CH_SIZES_BGR[1].width;
    size_bgr_1.width = VPSS_CH_SIZES_BGR[1].height;
  }
  if (CH_INDEXES_BGR[2]) {
    size_bgr_2.height = VPSS_CH_SIZES_BGR[2].width;
    size_bgr_2.width = VPSS_CH_SIZES_BGR[2].height;
  }

  ImagePackage image_package1(size_bgr_1, size_bgr_2, VPSS_CH_SIZES_NIR[0],
                              VPSS_CH_SIZES_NIR[1]);
  ImagePackage image_package2(size_bgr_1, size_bgr_2, VPSS_CH_SIZES_NIR[0],
                              VPSS_CH_SIZES_NIR[1]);
  PingPangBuffer<ImagePackage> pingpang_buffer(&image_package1,
                                               &image_package2);
  int frame_idx = 0;
  while (1) {
    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();

    pingpang_buffer.startWrite();
    ImagePackage *pPing = pingpang_buffer.getPing();
    if (pvpss_bgr_->getYuvFrame(pPing->img_bgr_small, 2)) {
      while(!pvpss_bgr_->getYuvFrame(pPing->img_bgr_large, 1))
      {
        QThread::usleep(1);
      }
      frame_idx++;
      pPing->frame_idx = frame_idx;
      pingpang_buffer.endWrite();

      std::chrono::steady_clock::time_point t2 =
          std::chrono::steady_clock::now();
      std::chrono::duration<double> time_span =
          std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
      std::cout << "getFrame: "
                << ": \t" << time_span.count() << "\tseconds." << std::endl;
      // pingpang_buffer.switchToPang();
      emit txFrame(&pingpang_buffer);

      // printf("tx0 threadId=%x   %x %d\n", QThread::currentThreadId(), pPing,
      // frame_idx);

    } else {
      pingpang_buffer.endWrite();
      QThread::usleep(1);
    }
  }
}
