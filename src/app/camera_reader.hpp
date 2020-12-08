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
  static CameraReader *get_instance();

  void start_sample();

  bool get_screen_size(int &width, int &height);

 private slots:
  void rx_finish();

 signals:
  void tx_frame(PingPangBuffer<ImagePackage> *buffer);

 private:
  CameraReader(QObject *parent = nullptr);
  ~CameraReader();

  void run();
  bool capture_frame(ImagePackage *pkg);
  bool isp_update();

  bool rx_finished_;

  ImagePackage *buffer_ping_, *buffer_pang_;
  PingPangBuffer<ImagePackage> *pingpang_buffer_;
};

}  // namespace suanzi

#endif
