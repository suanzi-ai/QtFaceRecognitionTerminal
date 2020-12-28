#include "camera_reader.hpp"

#include <chrono>
#include <ctime>
#include <iostream>
#include <regex>

using namespace suanzi;
using namespace suanzi::io;

CameraReader *CameraReader::get_instance() {
  static CameraReader instance;
  return &instance;
}

CameraReader::CameraReader(QObject *parent) {
  auto app = Config::get_app();

  auto engine = Engine::instance();
  engine->start();

  // Initialize PINGPANG buffer
  Size size_bgr_1, size_bgr_2;
  Size size_nir_1, size_nir_2;

  engine->get_frame_size(CAMERA_BGR, 1, size_bgr_1);
  engine->get_frame_size(CAMERA_BGR, 2, size_bgr_2);
  engine->get_frame_size(CAMERA_NIR, 1, size_nir_1);
  engine->get_frame_size(CAMERA_NIR, 2, size_nir_2);

  buffer_ping_ =
      new ImagePackage(size_bgr_1, size_bgr_2, size_nir_1, size_nir_2);
  buffer_pang_ =
      new ImagePackage(size_bgr_1, size_bgr_2, size_nir_1, size_nir_2);
  pingpang_buffer_ =
      new PingPangBuffer<ImagePackage>(buffer_ping_, buffer_pang_);

  rx_finished_ = true;
  read_cameral_ = true;
}

CameraReader::~CameraReader() {
  if (buffer_ping_) delete buffer_ping_;
  if (buffer_pang_) delete buffer_pang_;
  if (pingpang_buffer_) delete pingpang_buffer_;
}

bool CameraReader::get_screen_size(int &width, int &height) {
  auto engine = Engine::instance();
  Size size;
  SZ_RETCODE ret = engine->get_screen_size(size);
  if (ret != SZ_RETCODE_OK) {
    return false;
  }
  width = size.width;
  height = size.height;
  return true;
}

void CameraReader::start_sample() { start(); }

void CameraReader::rx_finish() { rx_finished_ = true; }

bool CameraReader::capture_frame(ImagePackage *pkg) {
  auto engine = Engine::instance();
  static int frame_idx = 0;

  SZ_RETCODE ret;

  {
    ret = engine->capture_frame(io::CAMERA_BGR, 2, *pkg->img_bgr_small);
    if (ret != SZ_RETCODE_OK) return false;

    ret = SZ_RETCODE_FAILED;
    while (ret != SZ_RETCODE_OK) {
      ret = engine->capture_frame(io::CAMERA_BGR, 1, *pkg->img_bgr_large);
      QThread::usleep(10);
    }
  }
  {
    ret = engine->capture_frame(io::CAMERA_NIR, 2, *pkg->img_nir_small);
    if (ret != SZ_RETCODE_OK) return false;

    ret = SZ_RETCODE_FAILED;
    while (ret != SZ_RETCODE_OK) {
      ret = engine->capture_frame(io::CAMERA_NIR, 1, *pkg->img_nir_large);
      QThread::usleep(10);
    }
  }

  pkg->frame_idx = frame_idx++;

  return true;
}

void CameraReader::run() {
  while (!capture_frame(pingpang_buffer_->get_pang()))
    ;
  while (!capture_frame(pingpang_buffer_->get_ping()))
    ;

  while (true) {
    ImagePackage *output = pingpang_buffer_->get_ping();

    if (read_cameral_ && capture_frame(output) && rx_finished_) {
      rx_finished_ = false;
      emit tx_frame(pingpang_buffer_);
    } else {
      QThread::usleep(10);
    }
  }
}

void CameraReader::start_read_cameral() { read_cameral_ = true; }

void CameraReader::stop_read_cameral() { read_cameral_ = false; }
