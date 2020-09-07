#include "camera_reader.hpp"

#include <QDebug>
#include <chrono>
#include <ctime>
#include <iostream>
#include <regex>

using namespace suanzi;
using namespace suanzi::io;

CameraReader::CameraReader(QObject *parent) {
  auto app = Config::get_app();

  if (!load_screen_type()) {
    SZ_LOG_ERROR("Load screen type error");
  }

  // Initialize VI_VPSS for BGR
  auto bgr_cam = Config::get_camera(CAMERA_BGR);
  // Initialize VI_VPSS for NIR
  auto nir_cam = Config::get_camera(CAMERA_NIR);

  EngineOption opt = {
      .bgr =
          {
              .dev = bgr_cam.index,
              .pipe = bgr_cam.pipe,
              .flip = true,
              .vpss_group = bgr_cam.index,
              .channels =
                  {
                      VpssChannel{
                          .index = 0,
                          .rotate = 0,
                          .size =
                              {
                                  .width = 1920,
                                  .height = 1080,
                              },
                      },
                      VpssChannel{
                          .index = 1,
                          .rotate = 1,
                          .size =
                              {
                                  .width = 1080,
                                  .height = 704,
                              },
                      },
                      VpssChannel{
                          .index = 2,
                          .rotate = 1,
                          .size =
                              {
                                  .width = 320,
                                  .height = 224,
                              },
                      },
                  },
          },
      .nir =
          {
              .dev = nir_cam.index,
              .pipe = nir_cam.pipe,
              .flip = true,
              .vpss_group = nir_cam.index,
              .channels =
                  {
                      VpssChannel{
                          .index = 0,
                          .rotate = 0,
                          .size =
                              {
                                  .width = 1920,
                                  .height = 1080,
                              },
                      },
                      VpssChannel{
                          .index = 1,
                          .rotate = 1,
                          .size =
                              {
                                  .width = 1080,
                                  .height = 704,
                              },
                      },
                      VpssChannel{
                          .index = 2,
                          .rotate = 1,
                          .size =
                              {
                                  .width = 320,
                                  .height = 224,
                              },
                      },
                  },
          },
      .screen =
          {
              .type = lcd_screen_type_,
          },
      .show_nir = app.show_infrared_window,
  };

  Engine::instance()->init(opt);

  SZ_LOG_INFO("Update isp ...");
  if (!isp_update()) {
    SZ_LOG_ERROR("Update isp failed");
  }

  Config::get_instance()->appendListener("reload", [&]() {
    SZ_LOG_INFO("Update isp ...");
    if (!isp_update()) {
      SZ_LOG_ERROR("Update isp failed");
    }
  });

  // Initialize PINGPANG buffer
  Size size_bgr_1 = opt.bgr.channels[1].size;
  Size size_bgr_2 = opt.bgr.channels[2].size;
  if (opt.bgr.channels[1].rotate % 2) {
    size_bgr_1.height = opt.bgr.channels[1].size.width;
    size_bgr_1.width = opt.bgr.channels[1].size.height;
  }
  if (opt.bgr.channels[2].rotate % 2) {
    size_bgr_2.height = opt.bgr.channels[2].size.width;
    size_bgr_2.width = opt.bgr.channels[2].size.height;
  }

  Size size_nir_1 = opt.nir.channels[1].size;
  Size size_nir_2 = opt.nir.channels[2].size;
  if (opt.nir.channels[1].rotate % 2) {
    size_nir_1.height = opt.nir.channels[1].size.width;
    size_nir_1.width = opt.nir.channels[1].size.height;
  }
  if (opt.nir.channels[2].rotate % 2) {
    size_nir_2.height = opt.nir.channels[2].size.width;
    size_nir_2.width = opt.nir.channels[2].size.height;
  }

  buffer_ping_ =
      new ImagePackage(size_bgr_1, size_bgr_2, size_nir_1, size_nir_2);
  buffer_pang_ =
      new ImagePackage(size_bgr_1, size_bgr_2, size_nir_1, size_nir_2);
  pingpang_buffer_ =
      new PingPangBuffer<ImagePackage>(buffer_ping_, buffer_pang_);

  rx_finished_ = true;
}

CameraReader::~CameraReader() {
  if (buffer_ping_) delete buffer_ping_;
  if (buffer_pang_) delete buffer_pang_;
  if (pingpang_buffer_) delete pingpang_buffer_;
}

bool CameraReader::load_screen_type() {
  std::string conf_filename = "/userdata/user.conf";
  std::ifstream conf(conf_filename);
  if (!conf.is_open()) {
    SZ_LOG_ERROR("Can't open {}", conf_filename);
    return false;
  }

  std::string line;
  std::regex reg("^LCD=(\\d+).+", std::regex_constants::ECMAScript |
                                      std::regex_constants::icase);
  std::smatch matches;
  while (std::getline(conf, line)) {
    if (std::regex_match(line, matches, reg)) {
      if (matches.size() == 2) {
        std::ssub_match base_sub_match = matches[1];
        std::string lcd_type = base_sub_match.str();
        // 0: mipi-7inch-1024x600(default)
        // 1: mipi-8inch-800x1280
        // 2: mipi-10inch-800x1280
        // 4: mipi-5inch-480x854

        if (lcd_type == "1") {
          lcd_screen_type_ = SML_LCD_MIPI_8INCH_800X1280;
          SZ_LOG_INFO("Load screen type SML_LCD_MIPI_8INCH_800X1280");
          return true;
        } else if (lcd_type == "4") {
          lcd_screen_type_ = SML_LCD_MIPI_5INCH_480X854;
          SZ_LOG_INFO("Load screen type SML_LCD_MIPI_5INCH_480X854");
          return true;
        } else {
          SZ_LOG_ERROR("lcd type unknown {}", lcd_type);
        }
        return false;
      }
    }
  }

  return false;
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

bool CameraReader::isp_update() {
  auto engine = Engine::instance();
  {
    auto cfg = Config::get_camera(CAMERA_BGR);
    SZ_RETCODE ret = engine->isp_update(CAMERA_BGR, &cfg.isp);
    if (ret != SZ_RETCODE_OK) return false;
  }
  {
    auto cfg = Config::get_camera(CAMERA_NIR);
    SZ_RETCODE ret = engine->isp_update(CAMERA_NIR, &cfg.isp);
    if (ret != SZ_RETCODE_OK) return false;
  }
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

    if (capture_frame(output) && rx_finished_) {
      rx_finished_ = false;
      emit tx_frame(pingpang_buffer_);
    } else {
      QThread::usleep(10);
    }
  }
}
