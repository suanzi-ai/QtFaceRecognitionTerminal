#include "camera_reader.hpp"

#include <QDebug>
#include <chrono>
#include <ctime>
#include <iostream>

#include "quface/common.hpp"
#include "vi.h"

using namespace suanzi;

CameraReader::CameraReader(QObject *parent) {
  pvi_bgr_ = new Vi(DEV_IDX_BRG, PIPE_IDX_BRG, SONY_IMX307_MIPI_2M_30FPS_12BIT);
  pvpss_bgr_ = new Vpss(DEV_IDX_BRG, VPSS_CH_SIZES_BGR[0].width,
                        VPSS_CH_SIZES_BGR[0].height);
  pvi_vpss_bgr_ =
      new Vi_Vpss(pvi_bgr_, pvpss_bgr_, VPSS_CH_SIZES_BGR, CH_INDEXES_BGR,
                  CH_ROTATES_BGR, sizeof(VPSS_CH_SIZES_BGR) / sizeof(Size));

  pvi_nir_ = new Vi(DEV_IDX_NIR, PIPE_IDX_NIR, SONY_IMX307_MIPI_2M_30FPS_12BIT);
  pvpss_nir_ = new Vpss(DEV_IDX_NIR, VPSS_CH_SIZES_NIR[0].width,
                        VPSS_CH_SIZES_NIR[0].height);
  pvi_vpss_nir_ =
      new Vi_Vpss(pvi_nir_, pvpss_nir_, VPSS_CH_SIZES_NIR, CH_INDEXES_NIR,
                  CH_ROTATES_NIR, sizeof(VPSS_CH_SIZES_NIR) / sizeof(Size));

  static Vo vo_bgr(0, VO_INTF_MIPI, VO_W, VO_H);
  static Vi_Vpss_Vo vi_vpss_vo(pvi_vpss_bgr_, pvi_vpss_nir_, &vo_bgr);
  b_tx_ok_ = true;
}

CameraReader::~CameraReader() {
  delete pvi_bgr_;
  delete pvpss_bgr_;
  delete pvi_vpss_bgr_;
  delete pvi_nir_;
  delete pvpss_nir_;
  delete pvi_vpss_nir_;
}

void CameraReader::start_sample() { start(); }

void CameraReader::rx_finish() { b_tx_ok_ = true; }

void CameraReader::run() {
  Size size_bgr_1 = VPSS_CH_SIZES_BGR[1];
  Size size_bgr_2 = VPSS_CH_SIZES_BGR[2];
  if (CH_ROTATES_BGR[1]) {
    size_bgr_1.height = VPSS_CH_SIZES_BGR[1].width;
    size_bgr_1.width = VPSS_CH_SIZES_BGR[1].height;
  }
  if (CH_ROTATES_BGR[2]) {
    size_bgr_2.height = VPSS_CH_SIZES_BGR[2].width;
    size_bgr_2.width = VPSS_CH_SIZES_BGR[2].height;
  }

  Size size_nir_1 = VPSS_CH_SIZES_NIR[1];
  Size size_nir_2 = VPSS_CH_SIZES_NIR[2];
  if (CH_ROTATES_NIR[1]) {
    size_nir_1.height = VPSS_CH_SIZES_NIR[1].width;
    size_nir_1.width = VPSS_CH_SIZES_NIR[1].height;
  }
  if (CH_ROTATES_NIR[2]) {
    size_nir_2.height = VPSS_CH_SIZES_NIR[2].width;
    size_nir_2.width = VPSS_CH_SIZES_NIR[2].height;
  }

  ImagePackage image_package1(size_bgr_1, size_bgr_2, size_nir_1, size_nir_2);
  ImagePackage image_package2(size_bgr_1, size_bgr_2, size_nir_1, size_nir_2);
  PingPangBuffer<ImagePackage> pingpang_buffer(&image_package1,
                                               &image_package2);
  int frame_idx = 0;
  bool b_data_ready = false;
  while (1) {
    ImagePackage *pPing = pingpang_buffer.get_ping();
    if (pvpss_bgr_->getYuvFrame(pPing->img_bgr_small, 2) &&
        pvpss_nir_->getYuvFrame(pPing->img_nir_small, 2)) {
      while (!pvpss_bgr_->getYuvFrame(pPing->img_bgr_large, 1) &&
             pvpss_nir_->getYuvFrame(pPing->img_nir_large, 1)) {
        QThread::usleep(1000);
      }

      if (b_tx_ok_) {
        pPing->frame_idx = frame_idx++;
        b_tx_ok_ = false;
        b_data_ready = false;
        // SZ_LOG_DEBUG("tx_frame");
        emit tx_frame(&pingpang_buffer);
      } else {
        b_data_ready = true;
      }
    } else {
      if (b_data_ready && b_tx_ok_) {
        b_tx_ok_ = false;
        b_data_ready = false;
        // SZ_LOG_DEBUG("tx_frame");
        pPing->frame_idx = frame_idx++;
        emit tx_frame(&pingpang_buffer);
        // printf("tx threadId=%x  %x %d\n", QThread::currentThreadId(), pPing,
        //        pPing->frame_idx);
      }
      QThread::usleep(1000);
    }
  }
}
