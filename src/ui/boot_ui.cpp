#include "boot_ui.hpp"
#include <opencv2/opencv.hpp>

using namespace suanzi;
using namespace suanzi::io;
BootUi::BootUi() {
	b_exit_ = false;
	pvo_ = new Vo(0, VO_INTF_MIPI, SML_LCD_MIPI_8INCH_800X1280, 800, 1280);
	pvo_->start(VO_MODE_1MUX);
	start();
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
	cv::resize(img, img, {800, 1280});
	while(!b_exit_) {
		pvo_->sendFrame(img);
		usleep(250000);
	}
}
