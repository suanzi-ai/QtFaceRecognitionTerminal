#include "camera_reader.hpp"

#include <QDebug>
#include <chrono>
#include <ctime>
#include <iostream>
#include <regex>

#include "quface/common.hpp"

using namespace suanzi;

CameraReader::CameraReader(QObject *parent) {
  auto app = Config::get_app();

  if (!load_screen_type()) {
    SZ_LOG_ERROR("Load screen type error");
  }

  // Initialize VI_VPSS for BGR
  vi_bgr_ = new Vi(DEV_IDX_BRG, PIPE_IDX_BRG, SONY_IMX307_MIPI_2M_30FPS_12BIT,
                   BGR_FLIP);
  vpss_bgr_ = new Vpss(DEV_IDX_BRG, VPSS_CH_SIZES_BGR[0].width,
                       VPSS_CH_SIZES_BGR[0].height);
  vi_vpss_bgr_ =
      new Vi_Vpss(vi_bgr_, vpss_bgr_, VPSS_CH_SIZES_BGR, CH_INDEXES_BGR,
                  CH_ROTATES_BGR, sizeof(VPSS_CH_SIZES_BGR) / sizeof(Size));

  // Initialize VI_VPSS for NIR
  vi_nir_ = new Vi(DEV_IDX_NIR, PIPE_IDX_NIR, SONY_IMX307_MIPI_2M_30FPS_12BIT,
                   NIR_FLIP);
  vpss_nir_ = new Vpss(DEV_IDX_NIR, VPSS_CH_SIZES_NIR[0].width,
                       VPSS_CH_SIZES_NIR[0].height);
  vi_vpss_nir_ =
      new Vi_Vpss(vi_nir_, vpss_nir_, VPSS_CH_SIZES_NIR, CH_INDEXES_NIR,
                  CH_ROTATES_NIR, sizeof(VPSS_CH_SIZES_NIR) / sizeof(Size));
  static Vo vo_bgr(0, VO_INTF_MIPI, lcd_screen_type_, ROTATION_E::ROTATION_90);
  if (app.show_infrared_window) {
    static Vi_Vpss_Vo vi_vpss_vo(vi_vpss_bgr_, vi_vpss_nir_, &vo_bgr);
  } else {
    static Vi_Vpss_Vo vi_vpss_vo(vi_vpss_bgr_, &vo_bgr);
  }

  // Initialize PINGPANG buffer
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

  buffer_ping_ =
      new ImagePackage(size_bgr_1, size_bgr_2, size_nir_1, size_nir_2);
  buffer_pang_ =
      new ImagePackage(size_bgr_1, size_bgr_2, size_nir_1, size_nir_2);
  pingpang_buffer_ =
      new PingPangBuffer<ImagePackage>(buffer_ping_, buffer_pang_);

  rx_finished_ = true;
}

CameraReader::~CameraReader() {
  if (vi_bgr_) delete vi_bgr_;
  if (vpss_bgr_) delete vpss_bgr_;
  if (vi_vpss_bgr_) delete vi_vpss_bgr_;

  if (vi_nir_) delete vi_nir_;
  if (vpss_nir_) delete vpss_nir_;
  if (vi_vpss_nir_) delete vi_vpss_nir_;

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
  return get_screen_width_height(lcd_screen_type_, width, height);
}

void CameraReader::start_sample() { start(); }

void CameraReader::rx_finish() { rx_finished_ = true; }

bool CameraReader::capture_frame(ImagePackage *pkg) {
  static int frame_idx = 0;

  if (!vpss_bgr_->getYuvFrame(pkg->img_bgr_small, 2)) return false;
  while (!vpss_bgr_->getYuvFrame(pkg->img_bgr_large, 1)) {
    QThread::usleep(10);
  }

  if (Config::enable_anti_spoofing()) {
    if (!vpss_nir_->getYuvFrame(pkg->img_nir_small, 2)) return false;
    while (!vpss_nir_->getYuvFrame(pkg->img_nir_large, 1)) {
      QThread::usleep(10);
    }
  }

  pkg->frame_idx = frame_idx++;

  return true;
}

void CameraReader::run() {
  while (true) {
    ImagePackage *ping = pingpang_buffer_->get_ping();

    if (capture_frame(ping) && rx_finished_) {
      rx_finished_ = false;
      emit tx_frame(pingpang_buffer_);
    } else {
      QThread::usleep(10);
    }
  }
}
