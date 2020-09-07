#ifndef CAMERA_READER_H
#define CAMERA_READER_H

#include <QImage>
#include <QSharedPointer>
#include <QThread>
#include <chrono>
#include <quface-io/engine.hpp>

#include "config.hpp"
#include "image_package.hpp"
#include "pingpang_buffer.hpp"

namespace suanzi {

class CameraReader : QThread {
  Q_OBJECT

 public:
  CameraReader(QObject *parent = nullptr);
  ~CameraReader();

  void start_sample();

  bool load_screen_type();
  bool get_screen_size(int &width, int &height);

 private slots:
  void rx_finish();

 signals:
  void tx_frame(PingPangBuffer<ImagePackage> *buffer);

 private:
  void run();
  bool capture_frame(ImagePackage *pkg);
  bool isp_update();

  bool rx_finished_;

  ImagePackage *buffer_ping_, *buffer_pang_;
  PingPangBuffer<ImagePackage> *pingpang_buffer_;

  io::LCDScreenType lcd_screen_type_;
};

}  // namespace suanzi

#endif
