#include "boot_ui.hpp"
#include <opencv2/opencv.hpp>
#include "config.hpp"
#include <regex>


using namespace suanzi;
using namespace suanzi::io;
BootUi::BootUi() {
	b_exit_ = false;
	int lcd_screen_type = 0;
	if (!load_screen_type(lcd_screen_type, width_, height_))
		return;
	pvo_ = new Vo(0, VO_INTF_MIPI, (io::LCDScreenType)lcd_screen_type, width_, height_);
	pvo_->start(VO_MODE_1MUX);
	start();
}



bool BootUi::load_screen_type(int &lcd_screen_type, int &width, int &height) {
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
          lcd_screen_type = SML_LCD_MIPI_8INCH_800X1280;
		  width = 800;
		  height = 1280;
          SZ_LOG_INFO("Load screen type SML_LCD_MIPI_8INCH_800X1280");
          return true;
        } else if (lcd_type == "4") {
          lcd_screen_type = SML_LCD_MIPI_5INCH_480X854;
		  width = 480;
		  height = 854;
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



BootUi::~BootUi() {
	delete pvo_;
}

void BootUi::stop() {
	b_exit_ = true;
	pvo_->stop();
}

void BootUi::run() {
	cv::Mat img = cv::imread("background.jpg");
	cv::resize(img, img, {width_, height_});
	while(!b_exit_) {
		pvo_->sendFrame(img);
		usleep(250000);
	}
}
