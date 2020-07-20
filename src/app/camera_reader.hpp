#ifndef CAMERA_READER_H
#define CAMERA_READER_H

#include <QImage>
#include <QSharedPointer>
#include <QThread>
#include <chrono>

#include "config.hpp"
#include "image_package.hpp"
#include "pingpang_buffer.hpp"
#include "vi.h"
#include "vi_vpss.h"
#include "vi_vpss_vo.h"
#include "vo.h"
#include "vpss.h"

namespace suanzi {

class CameraReader : QThread {
  Q_OBJECT

 public:
  CameraReader(QObject *parent = nullptr);
  ~CameraReader();

  void start_sample();

 private:
 private slots:
  void rx_finish();

 signals:
  void tx_frame(PingPangBuffer<ImagePackage> *buffer);

 private:
  const int DEV_IDX_BRG = 1;
  const int PIPE_IDX_BRG = 2;
  const int DEV_IDX_NIR = 0;
  const int PIPE_IDX_NIR = 0;

  const Size VPSS_CH_SIZES_BGR[3] = {
      {1920, 1080}, {1080, 704}, {320, 224}};  // larger small
  const Size VPSS_CH_SIZES_NIR[3] = {
      {1920, 1080}, {1080, 704}, {320, 224}};  // larger small
  const int CH_INDEXES_BGR[3] = {0, 1, 2};
  const bool CH_ROTATES_BGR[3] = {false, true, true};
  const int CH_INDEXES_NIR[3] = {0, 1, 2};
  const bool CH_ROTATES_NIR[3] = {false, true, true};

#if LCD_INCH_8
  const int VO_W = 800;
  const int VO_H = 1280;
#else  // LCD_INCH_5
  const int VO_W = 480;
  const int VO_H = 854;
#endif

  void run();
  bool capture_frame(ImagePackage *pkg);

  bool rx_finished_;

  Vi *vi_bgr_, *vi_nir_;
  Vpss *vpss_bgr_, *vpss_nir_;
  Vi_Vpss *vi_vpss_bgr_, *vi_vpss_nir_;

  ImagePackage *buffer_ping_, *buffer_pang_;
  PingPangBuffer<ImagePackage> *pingpang_buffer_;
};

}  // namespace suanzi

#endif
