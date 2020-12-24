#include <fstream>
#include <regex>
#include <string>
#include <thread>
#include <vector>

#include <mpi_sys.h>
#include <zbar.h>
#include <opencv2/opencv.hpp>

#include <QFile>

#include <quface-io/engine.hpp>
#include <quface/logger.hpp>

#include "config.hpp"
#include "system.hpp"

using namespace suanzi;
using namespace suanzi::io;

void trim(std::string& s) {
  s.erase(0, s.find_first_not_of(" "));
  s.erase(s.find_last_not_of(" ") + 1);
  s.erase(s.find_last_not_of("\n") + 1);
}

std::string zbar_decoder(const cv::Mat& img) {
  zbar::ImageScanner scanner;
  const void* raw = img.data;
  // configure the reader
  scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);
  scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_X_DENSITY, 1);
  scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_Y_DENSITY, 1);

  // wrap image data
  zbar::Image image(img.cols, img.rows, "Y800", raw, img.cols * img.rows);
  // scan the image for barcodes
  int n = scanner.scan(image);
  // extract results

  zbar::Image::SymbolIterator symbol = image.symbol_begin();
  std::string result = symbol->get_data();
  image.set_data(NULL, 0);
  return result;
}

std::string return_code_from_binary_image(const cv::Mat& image) {
  std::string result = zbar_decoder(image);
  if (result.empty()) {
    cv::Mat open_img;
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::morphologyEx(image, open_img, cv::MORPH_OPEN, element);
    result = zbar_decoder(open_img);
  }
  return result;
}

std::string RETURN_CODE(const cv::Mat& image) {
  static cv::Mat binary_image;

  cv::adaptiveThreshold(image, binary_image, 255,
                        cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 33,
                        0);

  return return_code_from_binary_image(binary_image);
}

bool SCAN_QRCODE() {
  const int screen_width = 800;
  const int screen_height = 1280;
  const int QRCODE_WIDTH = 400;
  const int QRCODE_HEIGHT = 400;

  auto engine = Engine::instance();

  static MmzImage* mmz_yuv_image = NULL;
  if (mmz_yuv_image == NULL)
    mmz_yuv_image = new MmzImage(1072, 1728, SZ_IMAGETYPE_NV21);

  if (SZ_RETCODE_OK ==
      engine->capture_frame(io::CAMERA_BGR, 0, *mmz_yuv_image)) {
    static cv::Mat qr_image;
    static int index = 0;

    cv::Mat yuvImage(mmz_yuv_image->height, mmz_yuv_image->width, CV_8UC1,
                     mmz_yuv_image->pData);
    yuvImage(cv::Rect((screen_width - QRCODE_WIDTH) * 1.35 / 2,
                      (screen_height - QRCODE_HEIGHT) * 1.35 / 2,
                      QRCODE_WIDTH * 1.35, QRCODE_HEIGHT * 1.35))
        .copyTo(qr_image);

    cv::flip(qr_image, qr_image, 1);

    std::string qr_code = RETURN_CODE(qr_image);
    std::string r;
    if (!qr_code.empty()) {
      trim(qr_code);
      auto pos = qr_code.find(' ');
      if (pos != std::string::npos) {
        auto mac_address = qr_code.substr(0, pos);
        auto sn = qr_code.substr(pos + 1);
        if (mac_address.length() == 12 && sn.length() == 11) {
          std::transform(mac_address.begin(), mac_address.end(),
                         mac_address.begin(),
                         [](unsigned char c) { return std::tolower(c); });
          for (size_t i = 5; i > 0; i--) mac_address.insert(i * 2, ":");
          SZ_LOG_INFO("mac={}, sn={}", mac_address, sn);

          char cmd[100];
          std::string output;
          sprintf(cmd, "echo %s > /etc/serial-number", sn.c_str());
          if (SZ_RETCODE_OK != System::exec(cmd, output)) {
            SZ_LOG_ERROR("Save serial-number failed: {}", output);
            return false;
          }

          sprintf(cmd, "fw_setenv ethaddr %s", mac_address.c_str());
          if (SZ_RETCODE_OK != System::exec(cmd, output)) {
            SZ_LOG_ERROR("Save mac address failed: {}", output);
            return false;
          }

          if (SZ_RETCODE_OK !=
              System::exec("sed -i '/qrcode-test/d' /usr/script/sample.sh && "
                           "echo '/user/quface-app/bin/supervisord -c "
                           "/user/quface-app/etc/supervisord.conf -d' >> "
                           "/usr/script/sample.sh",
                           output)) {
            SZ_LOG_ERROR("Update sample.sh failed: {}", output);
            return false;
          }

          return true;
        }
      }
    }
  }

  return false;
}

void INIT_ENGINE() {
  LCDScreenType screen_type = RH_9881_8INCH_800X1280;
  if (!Config::load_screen_type(screen_type)) return;

  SensorType sensor0_type = SONY_IMX327_2L_MIPI_2M_30FPS_12BIT;
  SensorType sensor1_type = SONY_IMX327_2L_MIPI_2M_30FPS_12BIT;
  if (!Config::load_sensor_type(sensor0_type, sensor1_type)) return;

  EngineOption opt = {
      .bgr =
          {
              .sensor_type = sensor0_type,
              .dev = 1,
              .flip = true,
              .wdr = false,
              .channels =
                  {
                      {
                          .index = 0,
                          .rotate = 3,
                          .size =
                              {
                                  .width = 1920,
                                  .height = 1080,
                              },
                      },
                  },
          },
      .nir =
          {
              .sensor_type = sensor1_type,
              .dev = 0,
              .flip = true,
              .wdr = false,
              .channels =
                  {
                      {
                          .index = 0,
                          .rotate = 3,
                          .size =
                              {
                                  .width = 1920,
                                  .height = 1080,
                              },
                      },
                  },
          },
      .screen =
          {
              .type = screen_type,
          },
      .show_secondary_win = true,
      .secondary_win_percent = SECONDARY_WIN_PERCENT_33,
  };

  auto engine = Engine::instance();

  engine->set_option(opt);
  engine->start();
}

int main() {
  HI_MPI_SYS_Init();
  INIT_ENGINE();

  std::string name = ":asserts/zh-CN/recognition_succeed.aac";
  std::vector<SZ_BYTE> audio;
  QFile audio_file(name.c_str());
  audio_file.open(QIODevice::ReadOnly);
  auto data = audio_file.readAll();
  audio.assign(data.begin(), data.end());

  auto engine = Engine::instance();
  engine->audio_set_volume(100);

  while (!SCAN_QRCODE()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  engine->audio_play(audio);
  for (int i = 0; i < 3; i++) {
    engine->gpio_set(GpioPinLightBox, true);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    engine->gpio_set(GpioPinLightBox, false);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  HI_MPI_SYS_Exit();

  std::string output;
  System::exec("reboot -d 10 &", output);

  return 0;
}
